#include "renderer.h"

#include "material.h"
#include "random.h"

#include <cstring>

#define NUM_SAMPLES 4
#if NUM_SAMPLES == 4
    #define USE_4X_MSAA
#endif
#ifdef USE_4X_MSAA
const std::vector<glm::vec2> Renderer::ms_samples = { glm::vec2(1.f/8.f, 5.f/8.f), glm::vec2(3.f/8.f, 1.f/8.f), glm::vec2(5.f/8.f, 7.f/8.f), glm::vec2(7.f/8.f, 3.f/8.f) };
#elif NUM_SAMPLES > 1
// perform random sampling
const std::vector<glm::vec2> Renderer::ms_samples = []()
    { 
        std::vector<glm::vec2> samples;
        for (int i = 0; i < NUM_SAMPLES; ++i)
        {
            samples.push_back(glm::vec2(GetNextRandom(), GetNextRandom()));
        }
        return samples;
    }();
#else
const std::vector<glm::vec2> Renderer::ms_samples = { glm::vec2(0.5f, 0.5f) };
#endif

const float Renderer::ms_weightingFactor = 1.f / static_cast<float>(Renderer::ms_samples.size());

constexpr int NUM_LINES_PER_RENDER_TASK = 12;
constexpr int NUM_MAX_REFINEMENTS = 2048;

Renderer::Renderer(const Viewport& v) 
: m_threadPool()
, m_viewport(v)
, m_currentRefinementIteration(0)
{
    // compute number of tasks for multi-threaded rendering
    int numTasks = m_viewport.GetHeight() / NUM_LINES_PER_RENDER_TASK;
    if (m_viewport.GetHeight() % NUM_LINES_PER_RENDER_TASK != 0)
    {
        numTasks++;
    }

    m_numRenderTasks = numTasks;

    // resize and initialize the accumulated frame buffer
    m_accumulationBuffer.resize(m_viewport.GetHeight() * m_viewport.GetWidth());
    ClearFramebuffer();
}    

/// Just a simple gradient for background color
glm::vec3 Renderer::BackgroundColor(const Ray& r) const
{
    glm::vec3 unitDirection = glm::normalize(r.Direction());
    float t = 0.5f * (unitDirection.y + 1.f);

    return glm::mix(glm::vec3(1.f), glm::vec3(0.5f, 0.7f, 1.f), t);
}

/// recursive single-bounce diffuse reflection color computation
glm::vec3 Renderer::ComputeColor(const Ray& r, const Hitable& world, int depth) const
{
    constexpr float EPSILON = 0.0001f;
    constexpr int MAX_DEPTH = 50;

    HitRecord rec;
    if (world.Hit(r, EPSILON, std::numeric_limits<float>::max(), rec))
    {
        Ray scattered(glm::vec3(0.f), glm::vec3(0.f));
        glm::vec3 attenuation;

        if (depth < MAX_DEPTH && rec.material->Scatter(r, rec, attenuation, scattered))
        {
            return attenuation * ComputeColor(scattered, world, depth+1);
        }
        else
        {
            return glm::vec3(0.f);
        }
    }
    else
    {
        return BackgroundColor(r);
    }
}

void Renderer::ClearFramebuffer()
{
    std::memset(m_accumulationBuffer.data(), 0, m_accumulationBuffer.size() * sizeof(glm::vec3));
    m_currentRefinementIteration = 0;
}

void Renderer::Render(const Hitable& world, uint32_t* pixelData)
{
    if (m_currentRefinementIteration < NUM_MAX_REFINEMENTS)
    {
        const Camera& camera = m_trackball.GetCamera();

        glm::vec3 lowerLeft = camera.GetOrigin() + camera.GetDirection() - camera.GetRight() * m_viewport.GetHorizontalLinearFov() - camera.GetUp();
        glm::vec3 vertical = 2.f * camera.GetUp();
        glm::vec3 horizontal = 2.f * camera.GetRight() * m_viewport.GetHorizontalLinearFov();

        // set number of tasks the thread pool should process (in the current frame)
        m_threadPool.SetTaskCounter(m_numRenderTasks);

        for (int t = 0; t < m_numRenderTasks - 1; ++t)
        {
            int minLine = t * NUM_LINES_PER_RENDER_TASK;
            int maxLine = (t + 1) * NUM_LINES_PER_RENDER_TASK;

            m_threadPool.AddTask(RenderTask{ this, minLine, maxLine, camera, world, lowerLeft, vertical, horizontal });
        }
        // last task can have less lines, so we handle it explicitly
        m_threadPool.AddTask(RenderTask{ this, (m_numRenderTasks - 1) * NUM_LINES_PER_RENDER_TASK, m_viewport.GetHeight(), camera, world, lowerLeft, vertical, horizontal });
    
        // wait for all tasks to finish
        m_threadPool.WaitForTasks();
    
        m_currentRefinementIteration++;
    }

    SetAccumulatedImage(pixelData);
}

void Renderer::SetAccumulatedImage(uint32_t* pixelData)
{
    float invRefinements = 1.f / static_cast<float>(m_currentRefinementIteration);

    for (size_t index = 0; index < m_accumulationBuffer.size(); ++index)
    {
        glm::vec3 color = m_accumulationBuffer[index] * invRefinements;
    
        uint8_t rc = static_cast<uint8_t>(color.r * 255.f);
        uint8_t gc = static_cast<uint8_t>(color.g * 255.f);
        uint8_t bc = static_cast<uint8_t>(color.b * 255.f);

        uint32_t pixel = 0xFF << 24;  // full alpha
        pixel |= (static_cast<uint32_t>(rc) << 0);
        pixel |= (static_cast<uint32_t>(gc) << 8);
        pixel |= (static_cast<uint32_t>(bc) << 16);

        pixelData[index] = pixel;
    }
}

void Renderer::RenderLines(int min, int max, const Camera& camera, const Hitable& world, glm::vec3 lowerLeft, glm::vec3 vertical, glm::vec3 horizontal)
{
    for (int j = max - 1; j >= min; --j)
    {
        // note that pixels start at upper left in SDL2 buffer
        int lineOffset = (m_viewport.GetHeight() - 1 - j)*m_viewport.GetWidth();

        for (int i = 0; i < m_viewport.GetWidth(); ++i)
        {
            glm::vec2 pixelCoord(static_cast<float>(i), static_cast<float>(j));

            glm::vec3 color = glm::vec3(0.f);

            for (auto& subpixelOffset : ms_samples)
            {
                glm::vec2 sampleCoord = pixelCoord + subpixelOffset;
                sampleCoord *= m_viewport.GetViewportSizeRcp();

                Ray r(camera.GetOrigin(), glm::normalize(lowerLeft + sampleCoord.x * horizontal + sampleCoord.y * vertical - camera.GetOrigin()));

                color += ComputeColor(r, world);
            }

            color *= ms_weightingFactor;

            GammaCorrection(color);
 
            int index = lineOffset + i;

            m_accumulationBuffer[index] += color;
        }
    }
}
