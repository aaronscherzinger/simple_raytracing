#include "commonheader.h"

#include "camera.h"
#include "dielectric.h"
#include "hitablelist.h"
#include "lambertian.h"
#include "metal.h"
#include "random.h"
#include "renderer.h"
#include "sphere.h"
#include "viewport.h"

// helper function to check if two spheres intersect
bool Intersect(glm::vec3 center1, float radius1, glm::vec3 center2, float radius2)
{
    return glm::distance(center1, center2) <= (radius1 + radius2);
}

int main()
{
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Init failed", SDL_GetError(), NULL);
        return -1;
    }

    // create a window
    int width = 600;
    int height = 400;
    SDL_Window *window;

    window = SDL_CreateWindow(
        "Simple Raytracing",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        //SDL_WINDOW_FULLSCREEN_DESKTOP
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create a window: %s.", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // create a drawing surface
    SDL_Surface* s;
    Uint32 rMask, gMask, bMask, aMask;

    // SDL interprets each pixel as a 32-bit number, so masks depend on the endianness of the system
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    static_assert(false, "SDL byte order must be little endian!");
#else
    rMask = 0x000000ff;
    gMask = 0x0000ff00;
    bMask = 0x00ff0000;
    aMask = 0xff000000;
#endif

    s = SDL_CreateRGBSurface(0, width, height, 32, rMask, gMask, bMask, aMask);

    if (s == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create a window: %s.", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Create a few materials
    std::vector<Lambertian> lambertians;
    lambertians.push_back(Lambertian(glm::vec3(0.5f)));

    std::vector<Metal> metals;
    metals.push_back(Metal(glm::vec3(0.8f), 0.01f));
    metals.push_back(Metal(glm::vec3(0.8f, 0.6f, 0.2f), 0.1f));

    std::vector<Dielectric> dielectrics;
    dielectrics.push_back(Dielectric(1.5f));

    // create more random materials
    for (int i = 0; i < 50; ++i)
    {
        lambertians.push_back(Lambertian(glm::vec3(GetNextRandom()*GetNextRandom(), GetNextRandom()*GetNextRandom(), GetNextRandom()*GetNextRandom())));
    }

    for (int i = 0; i < 50; ++i)
    {
        metals.push_back(Metal(glm::vec3(0.5f * (1.f + GetNextRandom()), 0.5f * (1.f + GetNextRandom()), 0.5f * (1.f + GetNextRandom())), 0.4f * GetNextRandom()));
    }

    // create a bunch of spheres
    std::vector<Sphere> spheres;

    // "floor"
    spheres.push_back(Sphere(glm::vec3(0.f, -301.f, 0.f), 300.f, &lambertians[0]));

    // larger spheres around the center
    spheres.push_back(Sphere(glm::vec3(0.f, 0.f, 0.f), 1.f, &metals[0]));
    spheres.push_back(Sphere(glm::vec3(2.2f, 0.15f, 0.f), 1.f, &metals[1])); 
    spheres.push_back(Sphere(glm::vec3(-2.1f, 0.1f, 0.4f), 0.8f, &dielectrics[0]));
    // trick: use negative radius make a bubble (hole)  within a glass sphere (i.e., a hollow glass sphere):
    spheres.push_back(Sphere(glm::vec3(-2.1f, 0.1f, 0.4f), -0.7f, &dielectrics[0]));

    // create a bunch of random smaller spheres
    size_t nextLambertian = 1;
    size_t nextMetal = 2;
    
    for (float a = -4.5f; a < 5.f; a += 1.5f)
    {
        for (float b = -3.f; b < 3.5f; b += 1.5f)
        {
            float radius = glm::clamp(0.5f * GetNextRandom(), 0.2f, 0.3f);
            glm::vec3 center = glm::vec3(a + 0.9f * GetNextRandom(), -1.f + radius, b + 0.9f * GetNextRandom());
            
            // avoid intersections with large spheres
            bool intersect = false;
            for (size_t i = 1; i < 4; ++i)
            {
                intersect |= Intersect(center, radius, spheres[i].GetCenter(), spheres[i].GetRadius());
            }
            if (intersect)
            {
                continue;
            }

            float chooseMaterial = GetNextRandom();

            if (chooseMaterial < 0.7f && nextLambertian < lambertians.size())
            {
                // diffuse
                spheres.push_back(Sphere(center, radius, &lambertians[nextLambertian++]));
            }
            else if (chooseMaterial < 0.95f && nextMetal < metals.size())
            {
                // metal
                spheres.push_back(Sphere(center, radius, &metals[nextMetal++]));
            }
            else
            {
                // glass
                spheres.push_back(Sphere(center, radius, &dielectrics[0]));
            }
        }
    }

    HitableList world;
    for (auto& s : spheres)
    {
        world.AddToList(&s);
    }

    // create a Renderer
    Viewport viewport(width, height);
    Renderer renderer(viewport);

    // set initial camera perspective
    renderer.GetTrackball().UpdateElevationAngle(-0.3f);
    renderer.GetTrackball().SetRadius(3.3f);
    renderer.GetTrackball().UpdatePolarAngle(0.5f);
    
    // render loop
    bool terminate = false;
    bool clearRendering = true;
    while (!terminate)
    {
        // Get the next event
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                // Break out of the loop on quit
                terminate = true;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                Trackball& trackball = renderer.GetTrackball();

                switch(event.key.keysym.sym) 
                {
                case SDLK_UP:
                {
                    if (event.key.keysym.mod & KMOD_LSHIFT)
                    {
                        if (trackball.GetRadius() > 0.15f)
                        {
                            trackball.SetRadius(trackball.GetRadius() - 0.1f);
                        }
                    }
                    else
                    {
                        trackball.UpdateElevationAngle(-0.1f);
                    }
                    clearRendering = true;
                    break;
                }

                case SDLK_DOWN:
                {
                    if (event.key.keysym.mod & KMOD_LSHIFT)
                    {
                        trackball.SetRadius(trackball.GetRadius() + 0.1f);
                    }
                    else
                    {
                        trackball.UpdateElevationAngle(0.1f);
                    }
                    clearRendering = true;
                    break;
                }

                case SDLK_LEFT:
                {
                    trackball.UpdatePolarAngle(0.1f);
                    clearRendering = true;
                    break;
                }

                case SDLK_RIGHT:
                {
                    trackball.UpdatePolarAngle(-0.1f);
                    clearRendering = true;
                    break;
                }

                default:
                break;
                }
            }
        }

        if (!terminate)
        {
            // when camera parameters changed, we need to clear the previous image
            if (clearRendering)
            {
                renderer.ClearFramebuffer();
                clearRendering = false;
            }

            SDL_LockSurface(s);
            uint32_t* pixelData = reinterpret_cast<uint32_t*>(s->pixels);

            // this will render the image or refine the rendering
            renderer.Render(world, pixelData);

            SDL_UnlockSurface(s);

            // blit the drawing surface to the window surface
            auto screen = SDL_GetWindowSurface(window);
            SDL_BlitSurface(s, nullptr, screen, nullptr);
        }

        SDL_UpdateWindowSurface(window);
    } 

    SDL_FreeSurface(s);
    s = nullptr;

    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
