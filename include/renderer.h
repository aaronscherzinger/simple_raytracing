#pragma once

#include "commonheader.h"

#include "camera.h"
#include "hitablelist.h"
#include "ray.h"
#include "renderthreadpool.h"
#include "viewport.h"

class Renderer final
{
public:
    Renderer() = delete;
    Renderer(const Viewport& v);

    Trackball& GetTrackball() { return m_trackball; }
    const Trackball& GetTrackball() const { return m_trackball; }
    void SetTrackball(const Trackball& trackball) { m_trackball = trackball; }

    Viewport& GetViewport() { return m_viewport; }
    const Viewport& GetViewport() const { return m_viewport; }
    void SetViewport(const Viewport& viewport) { m_viewport = viewport; }

    void ClearFramebuffer();
    void Render(const Hitable& world, uint32_t* pixelData);

protected:
    friend struct RenderTask;

    void RenderLines(int min, int max, const Camera& camera, const Hitable& world, glm::vec3 lowerLeft, glm::vec3 vertical, glm::vec3 horizontal);

private:
    // helper functions

    glm::vec3 BackgroundColor(const Ray& r) const;
    glm::vec3 ComputeFirstHitColor(const Ray& r, const Hitable& world) const;
    glm::vec3 ComputeColor(const Ray& r, const Hitable& world, int depth = 0) const;

    void GammaCorrection(glm::vec3& color) const {  color = glm::sqrt(color); }

    void SetAccumulatedImage(uint32_t* pixels);

    // internal framebuffer for accumulating multiple images
    std::vector<glm::vec3> m_accumulationBuffer;

    // threadpool for multi-threaded rendering
    RenderThreadPool m_threadPool;
    // trackball and viewport for camera and ray setup
    Trackball m_trackball;
    Viewport m_viewport;

    // number of tasks for rendering
    int m_numRenderTasks;
    // number of refinement iterations so far
    int m_currentRefinementIteration;

    // static vector containing subpixel offsets for MSAA
    const static std::vector<glm::vec2> ms_samples;
    // weighting factor for averaging subpixel samples
    const static float ms_weightingFactor;
};
