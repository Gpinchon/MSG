#include <ECS/Registry.hpp>

#include <SG/Entity/Camera.hpp>
#include <SG/Entity/Node.hpp>
#include <SG/Scene/Scene.hpp>
#include <SG/ShapeGenerator/Cube.hpp>

#include <Renderer/RenderBuffer.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/SwapChain.hpp>

#include <Tools/FPSCounter.hpp>
#include <Tools/ScopedTimer.hpp>

#include <SDL.h>
#include <SDL_syswm.h>
#include <functional>

using namespace TabGraph;

void PrintEvent(const SDL_Event* event)
{
    if (event->type == SDL_WINDOWEVENT) {
        switch (event->window.event) {
        case SDL_WINDOWEVENT_SHOWN:
            SDL_Log("Window %d shown", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_HIDDEN:
            SDL_Log("Window %d hidden", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_EXPOSED:
            SDL_Log("Window %d exposed", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_MOVED:
            SDL_Log("Window %d moved to %d,%d",
                event->window.windowID, event->window.data1,
                event->window.data2);
            break;
        case SDL_WINDOWEVENT_RESIZED:
            SDL_Log("Window %d resized to %dx%d",
                event->window.windowID, event->window.data1,
                event->window.data2);
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            SDL_Log("Window %d size changed to %dx%d",
                event->window.windowID, event->window.data1,
                event->window.data2);
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            SDL_Log("Window %d minimized", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            SDL_Log("Window %d maximized", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_RESTORED:
            SDL_Log("Window %d restored", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_ENTER:
            SDL_Log("Mouse entered window %d",
                event->window.windowID);
            break;
        case SDL_WINDOWEVENT_LEAVE:
            SDL_Log("Mouse left window %d", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            SDL_Log("Window %d gained keyboard focus",
                event->window.windowID);
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            SDL_Log("Window %d lost keyboard focus",
                event->window.windowID);
            break;
        case SDL_WINDOWEVENT_CLOSE:
            SDL_Log("Window %d closed", event->window.windowID);
            break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
        case SDL_WINDOWEVENT_TAKE_FOCUS:
            SDL_Log("Window %d is offered a focus", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_HIT_TEST:
            SDL_Log("Window %d has a special hit test", event->window.windowID);
            break;
#endif
        default:
            SDL_Log("Window %d got unknown event %d",
                event->window.windowID, event->window.event);
            break;
        }
    }
}

constexpr auto testWindowWidth  = 1280;
constexpr auto testWindowHeight = 800;

// int main(int argc, char const* argv[])
int SDL_main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    auto SDLWindow = SDL_CreateWindow(
        "TabGraph::UnitTests::SceneRenderer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        testWindowWidth, testWindowHeight, SDL_WINDOW_OPENGL);
    SDL_SysWMinfo wmInfo = {};
    wmInfo.version.major = SDL_MAJOR_VERSION;
    wmInfo.version.minor = SDL_MINOR_VERSION;
    wmInfo.version.patch = SDL_PATCHLEVEL;
    SDL_GetWindowWMInfo(SDLWindow, &wmInfo);
    HWND hwnd = wmInfo.info.win.window;
    // SDL_CreateWindowFrom(window.hwnd);

    int windowWidth = 0, windowHeight = 0;
    SDL_GetWindowSizeInPixels(SDLWindow, &windowWidth, &windowHeight);

    Renderer::CreateRenderBufferInfo renderBufferInfo;
    renderBufferInfo.width  = windowWidth;
    renderBufferInfo.height = windowHeight;

    Renderer::CreateSwapChainInfo swapChainInfo;
    swapChainInfo.hwnd           = hwnd;
    swapChainInfo.vSync          = false;
    swapChainInfo.imageCount     = 3;
    swapChainInfo.width          = windowWidth;
    swapChainInfo.height         = windowHeight;
    swapChainInfo.setPixelFormat = false; // SDL already did it

    auto registry     = ECS::DefaultRegistry::Create();
    auto renderer     = Renderer::Create({ "UnitTest", 100 });
    auto renderBuffer = Renderer::RenderBuffer::Create(renderer, renderBufferInfo);
    auto swapChain    = Renderer::SwapChain::Create(renderer, swapChainInfo);
    // auto window       = Window(renderer, testWindowWidth, testWindowHeight, false);
    // auto renderBuffer = Renderer::RenderBuffer::Create(renderer, { window.width, window.height });

    // build a test scene
    SG::Scene testScene(registry, "testScene");

    auto testCamera = SG::Camera::Create(registry);
    {
        auto testCube = SG::Cube::CreateMesh("testCube", { 1, 1, 1 });
        for (auto x = 0u; x < 5; ++x) {
            float xCoord = (x - (5 / 2.f)) * 2;
            for (auto y = 0u; y < 5; ++y) {
                float yCoord    = (y - (5 / 2.f)) * 2;
                auto testEntity = SG::Node::Create(registry);
                testEntity.AddComponent<SG::Component::Mesh>(testCube);
                testEntity.GetComponent<SG::Component::Transform>().SetPosition({ xCoord, yCoord, 0 });
                testScene.AddEntity(testEntity);
            }
        }
        SG::Component::Projection::PerspectiveInfinite cameraProj;
        cameraProj.fov                                              = 45.f;
        testCamera.GetComponent<SG::Component::Camera>().projection = cameraProj;
        testCamera.GetComponent<SG::Component::Transform>().SetPosition({ 5, 5, 5 });
        SG::Node::LookAt(testCamera, glm::vec3(0));
        testScene.AddEntity(testCamera);
        testScene.SetCamera(testCamera);
    }

    SDL_Event event = {};

    // window.OnResize = [&renderer, &renderBuffer, testCamera](Window&, uint32_t a_Width, uint32_t a_Height) mutable {
    //     renderBuffer = Renderer::RenderBuffer::Create(renderer, { a_Width, a_Height });
    //     SG::Component::Projection::PerspectiveInfinite projection;
    //     projection.aspectRatio                                      = a_Width / float(a_Height);
    //     testCamera.GetComponent<SG::Component::Camera>().projection = projection;
    //     Renderer::SetActiveRenderBuffer(renderer, renderBuffer);
    // };

    {
        Tools::ScopedTimer timer("Loading Test Scene");
        Renderer::Load(renderer, testScene);
    }

    Renderer::SetActiveScene(renderer, &testScene);
    Renderer::SetActiveRenderBuffer(renderer, renderBuffer);
    Renderer::Update(renderer);

    // window.Show();
    FPSCounter fpsCounter;
    auto lastTime   = std::chrono::high_resolution_clock::now();
    auto printTime  = lastTime;
    auto updateTime = lastTime;
    while (true) {
        while (SDL_PollEvent(&event)) {
            PrintEvent(&event);
        }

        // window.PushEvents();
        // if (window.closing)
        //     break;
        const auto now   = std::chrono::high_resolution_clock::now();
        const auto delta = std::chrono::duration<double, std::milli>(now - lastTime).count();
        lastTime         = now;
        fpsCounter.StartFrame();
        auto updateDelta = std::chrono::duration<double, std::milli>(now - updateTime).count();
        if (updateDelta >= 15) {
            updateTime = now;
            Renderer::Update(renderer);
        }
        Renderer::Render(renderer);
        Renderer::SwapChain::Present(swapChain, renderBuffer);
        // window.Present(renderBuffer);
        fpsCounter.EndFrame();
        if (std::chrono::duration<double, std::milli>(now - printTime).count() >= 48) {
            printTime = now;
            fpsCounter.Print();
        }
    }
    Renderer::Unload(renderer, testScene);
    return 0;
}
