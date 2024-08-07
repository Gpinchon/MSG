#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Assets/Parsers.hpp>
#include <Assets/Uri.hpp>
#include <ECS/Registry.hpp>
#include <Renderer/RenderBuffer.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/SwapChain.hpp>
#include <SG/Component/Light/PunctualLight.hpp>
#include <SG/Core/Image/Cubemap.hpp>
#include <SG/Core/Image/Image.hpp>
#include <SG/Entity/Camera.hpp>
#include <SG/Entity/Light/PunctualLight.hpp>
#include <SG/Entity/Node.hpp>
#include <SG/Scene/Scene.hpp>
#include <SG/ShapeGenerator/Cube.hpp>
#include <Tools/FPSCounter.hpp>
#include <Tools/ScopedTimer.hpp>

#define SDL_VIDEO_DRIVER_X11
#include <SDL.h>
#include <SDL_syswm.h>
#include <filesystem>
#include <functional>

using namespace TabGraph;

void PrintEvent(const SDL_Event& event)
{
    if (event.type == SDL_WINDOWEVENT) {
        switch (event.window.event) {
        case SDL_WINDOWEVENT_SHOWN:
            SDL_Log("Window %d shown", event.window.windowID);
            break;
        case SDL_WINDOWEVENT_HIDDEN:
            SDL_Log("Window %d hidden", event.window.windowID);
            break;
        case SDL_WINDOWEVENT_EXPOSED:
            SDL_Log("Window %d exposed", event.window.windowID);
            break;
        case SDL_WINDOWEVENT_MOVED:
            SDL_Log("Window %d moved to %d,%d",
                event.window.windowID, event.window.data1,
                event.window.data2);
            break;
        case SDL_WINDOWEVENT_RESIZED:
            SDL_Log("Window %d resized to %dx%d",
                event.window.windowID, event.window.data1,
                event.window.data2);
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            SDL_Log("Window %d size changed to %dx%d",
                event.window.windowID, event.window.data1,
                event.window.data2);
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            SDL_Log("Window %d minimized", event.window.windowID);
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            SDL_Log("Window %d maximized", event.window.windowID);
            break;
        case SDL_WINDOWEVENT_RESTORED:
            SDL_Log("Window %d restored", event.window.windowID);
            break;
        case SDL_WINDOWEVENT_ENTER:
            SDL_Log("Mouse entered window %d",
                event.window.windowID);
            break;
        case SDL_WINDOWEVENT_LEAVE:
            SDL_Log("Mouse left window %d", event.window.windowID);
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            SDL_Log("Window %d gained keyboard focus",
                event.window.windowID);
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            SDL_Log("Window %d lost keyboard focus",
                event.window.windowID);
            break;
        case SDL_WINDOWEVENT_CLOSE:
            SDL_Log("Window %d closed", event.window.windowID);
            break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
        case SDL_WINDOWEVENT_TAKE_FOCUS:
            SDL_Log("Window %d is offered a focus", event.window.windowID);
            break;
        case SDL_WINDOWEVENT_HIT_TEST:
            SDL_Log("Window %d has a special hit test", event.window.windowID);
            break;
#endif
        default:
            SDL_Log("Window %d got unknown event %d",
                event.window.windowID, event.window.event);
            break;
        }
    }
}
namespace Test {
class Window {
public:
    Window(const unsigned& a_Width, const unsigned& a_Height)
        : _sdlWindow(SDL_CreateWindow(
              "TabGraph::UnitTests::SceneRenderer",
              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
              a_Width, a_Height, 0)) // no flags because we want to set the pixel format ourselves
    {
        SDL_Event event;
        event.type = 0;
        while (event.type != SDL_WINDOWEVENT && event.window.event != SDL_WINDOWEVENT_EXPOSED)
            SDL_WaitEvent(&event);
    }
    ~Window()
    {
        SDL_DestroyWindow(_sdlWindow);
    }
    void ProcessEvent(const SDL_WindowEvent& a_Event)
    {
        switch (a_Event.event) {
        case SDL_WINDOWEVENT_SHOWN:
            if (onShown)
                return onShown(*this);
            break;
        case SDL_WINDOWEVENT_HIDDEN:
            if (onHidden)
                return onHidden(*this);
            break;
        case SDL_WINDOWEVENT_EXPOSED:
            if (onExposed)
                return onExposed(*this);
            break;
        case SDL_WINDOWEVENT_MOVED:
            if (onMoved)
                return onMoved(*this, a_Event.data1, a_Event.data2);
            break;
        case SDL_WINDOWEVENT_RESIZED:
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            if (onSizeChanged)
                return onSizeChanged(*this, a_Event.data1, a_Event.data2);
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            if (onMinimized)
                return onMinimized(*this);
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            if (onMaximized)
                return onMaximized(*this);
            break;
        case SDL_WINDOWEVENT_RESTORED:
            if (onRestored)
                return onRestored(*this);
            break;
        case SDL_WINDOWEVENT_ENTER:
            if (onEnter)
                return onEnter(*this);
            break;
        case SDL_WINDOWEVENT_LEAVE:
            if (onLeave)
                return onLeave(*this);
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            if (onFocusGained)
                return onFocusGained(*this);
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            if (onFocusLost)
                return onFocusLost(*this);
            break;
        case SDL_WINDOWEVENT_CLOSE:
            if (onClose)
                return onClose(*this);
            break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
        case SDL_WINDOWEVENT_TAKE_FOCUS:
            break;
        case SDL_WINDOWEVENT_HIT_TEST:
            break;
#endif
        default:
            SDL_Log("Window %d got unknown event %d",
                a_Event.windowID, a_Event.event);
            break;
        }
    }

    SDL_SysWMinfo GetWMInfo() const
    {
        SDL_SysWMinfo wmInfo = {};
        wmInfo.version.major = SDL_MAJOR_VERSION;
        wmInfo.version.minor = SDL_MINOR_VERSION;
        wmInfo.version.patch = SDL_PATCHLEVEL;
        SDL_GetWindowWMInfo(_sdlWindow, &wmInfo);
        return wmInfo;
    }

    unsigned GetID() const
    {
        return SDL_GetWindowID(_sdlWindow);
    }

    glm::uvec2 GetSize() const
    {
        int windowWidth  = 0;
        int windowHeight = 0;
        SDL_GetWindowSizeInPixels(_sdlWindow, &windowWidth, &windowHeight);
        return { windowWidth, windowHeight };
    }

public:
    std::function<void(Test::Window&, unsigned, unsigned)> onMoved;
    std::function<void(Test::Window&, unsigned, unsigned)> onSizeChanged;
    std::function<void(Test::Window&)> onShown;
    std::function<void(Test::Window&)> onHidden;
    std::function<void(Test::Window&)> onExposed;
    std::function<void(Test::Window&)> onMinimized;
    std::function<void(Test::Window&)> onMaximized;
    std::function<void(Test::Window&)> onRestored;
    std::function<void(Test::Window&)> onEnter;
    std::function<void(Test::Window&)> onLeave;
    std::function<void(Test::Window&)> onFocusGained;
    std::function<void(Test::Window&)> onFocusLost;
    std::function<void(Test::Window&)> onClose;

private:
    SDL_Window* _sdlWindow;
};

class Program {
public:
    Program(const unsigned& a_Width, const unsigned& a_Height)
        : window(a_Width, a_Height)
    {
    }
    ~Program() { SDL_Quit(); }
    void PollEvents()
    {
        SDL_Event event = {};
        while (SDL_PollEvent(&event)) {
            PrintEvent(event);
            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.windowID == window.GetID())
                    window.ProcessEvent(event.window);
            }
        }
    }

private:
    int _initialized = SDL_Init(SDL_INIT_EVERYTHING);

public:
    Test::Window window;
};
}

constexpr auto testWindowWidth  = 1920;
constexpr auto testWindowHeight = 1200;

struct Args {
    Args(const int& argc, char const* argv[])
    {
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "--env") {
                envPath = argv[i++];
            } else if (arg == "--model") {
                modelPath = argv[i++];
            }
        }
    }
    std::filesystem::path modelPath;
    std::filesystem::path envPath;
};

auto CreateSwapChain(
    const Renderer::Handle& a_Renderer,
    const Renderer::SwapChain::Handle& a_PrevSwapChain,
    const glm::uvec2& a_Size,
    const SDL_SysWMinfo& a_WMInfo)
{
    Renderer::CreateSwapChainInfo swapChainInfo;
    swapChainInfo.vSync      = false;
    swapChainInfo.imageCount = 3;
    swapChainInfo.width      = a_Size.x;
    swapChainInfo.height     = a_Size.y;
#ifdef WIN32
    swapChainInfo.windowInfo.hwnd = wmInfo.info.win.window;
#elifdef __linux
    swapChainInfo.windowInfo.display = a_WMInfo.info.x11.display;
    swapChainInfo.windowInfo.window  = a_WMInfo.info.x11.window;
#endif
    swapChainInfo.windowInfo.setPixelFormat = true;
    if (a_PrevSwapChain == nullptr)
        return Renderer::SwapChain::Create(a_Renderer, swapChainInfo);
    else
        return Renderer::SwapChain::Recreate(a_PrevSwapChain, swapChainInfo);
}

struct OrbitCamera {
    explicit OrbitCamera(std::shared_ptr<ECS::DefaultRegistry> const& a_Registry)
        : cameraEntity(SG::Camera::Create(a_Registry))
        , targetEntity(SG::Node::Create(a_Registry))
    {
        Update();
    }
    void Update() const
    {
        SG::Component::Projection::PerspectiveInfinite cameraProj;
        cameraProj.fov                                                = fov;
        cameraProj.aspectRatio                                        = aspectRatio;
        cameraEntity.GetComponent<SG::Component::Camera>().projection = cameraProj;
        SG::Node::Orbit(
            cameraEntity, targetEntity,
            radius, theta, phi);
    }
    float fov         = 45.f;
    float aspectRatio = testWindowWidth / float(testWindowHeight);
    float radius      = 1;
    float theta       = M_PI / 2.f;
    float phi         = M_PI;
    ECS::DefaultRegistry::EntityRefType cameraEntity;
    ECS::DefaultRegistry::EntityRefType targetEntity;
};

int main(int argc, char const* argv[])
{
    using namespace std::chrono_literals;
    auto args      = Args(argc, argv);
    args.envPath   = "/home/gpinchon/Projets/sunflowers_puresky.jpg";
    args.modelPath = "/home/gpinchon/Projets/glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf";
    if (args.envPath.empty() || args.modelPath.empty())
        return -1;
    auto registry = ECS::DefaultRegistry::Create();
    Assets::InitParsers();
    auto envAsset   = std::make_shared<Assets::Asset>(args.envPath);
    auto modelAsset = std::make_shared<Assets::Asset>(args.modelPath);
    envAsset->SetECSRegistry(registry);
    modelAsset->SetECSRegistry(registry);

    std::shared_ptr<SG::Scene> scene;
    OrbitCamera camera(registry);
    {
        auto model        = Assets::Parser::Parse(modelAsset);
        auto parsedScenes = model->Get<SG::Scene>();
        if (!parsedScenes.empty()) {
            scene = parsedScenes.front();
            scene->SetBackgroundColor({ 1, 1, 1 });
        } else {
            scene = std::make_shared<SG::Scene>(registry, "testScene");
            scene->SetBackgroundColor({ 1, 1, 1 });
        }
    }
    {
        auto env          = Assets::Parser::Parse(envAsset);
        auto parsedImages = env->GetCompatible<SG::Image>();
        if (!parsedImages.empty()) {
            auto& parsedImage = parsedImages.front();
            if (parsedImage->GetType() == SG::ImageType::Image2D) {
                auto lightIBLEntity = SG::PunctualLight::Create(registry);
                auto& lightIBLComp  = lightIBLEntity.GetComponent<SG::Component::PunctualLight>();
                auto cubemap        = std::make_shared<SG::Cubemap>(SG::Pixel::SizedFormat::Uint8_NormalizedRGB, 512, 512, *std::static_pointer_cast<SG::Image2D>(parsedImage));
                SG::Component::LightIBL lightIBLData;
                lightIBLData.intensity = 1;
                lightIBLData.specular  = cubemap;
                lightIBLComp           = lightIBLData;
                scene->AddEntity(lightIBLEntity);
                scene->SetSkybox(cubemap);
            }
        }
    }

    if (scene->GetCamera().Empty()) {
        scene->AddEntity(camera.cameraEntity);
        scene->AddEntity(camera.targetEntity);
        scene->SetCamera(camera.cameraEntity);
    }

    auto testProgram = Test::Program(testWindowWidth, testWindowHeight);
    auto wmInfo      = testProgram.window.GetWMInfo();
    auto windowSize  = testProgram.window.GetSize();

    Renderer::CreateRendererInfo rendererInfo {
        .name               = "UnitTest",
        .applicationVersion = 100,
#ifdef __linux
        .display = wmInfo.info.x11.display
#endif
    };
    Renderer::RendererSettings rendererSettings;

    Renderer::CreateRenderBufferInfo renderBufferInfo {
        .width  = windowSize.x,
        .height = windowSize.y
    };

    auto renderer     = Renderer::Create(rendererInfo, rendererSettings);
    auto renderBuffer = Renderer::RenderBuffer::Create(renderer, renderBufferInfo);
    auto swapChain    = CreateSwapChain(renderer, nullptr, windowSize, wmInfo);

    testProgram.window.onSizeChanged = [&renderer, &renderBuffer, &swapChain, &camera](Test::Window const& a_Window, uint32_t a_Width, uint32_t a_Height) mutable {
        renderBuffer       = Renderer::RenderBuffer::Create(renderer, { a_Width, a_Height });
        camera.aspectRatio = a_Width / float(a_Height);
        Renderer::SetActiveRenderBuffer(renderer, renderBuffer);
        swapChain = CreateSwapChain(renderer, swapChain, { a_Width, a_Height }, a_Window.GetWMInfo());
    };

    {
        Tools::ScopedTimer timer("Loading Test Scene");
        Renderer::Load(renderer, *scene);
    }

    Renderer::SetActiveScene(renderer, scene.get());
    Renderer::SetActiveRenderBuffer(renderer, renderBuffer);
    Renderer::Update(renderer);
    FPSCounter fpsCounter;
    bool closing               = false;
    auto lastTime              = std::chrono::high_resolution_clock::now();
    auto printTime             = lastTime;
    auto updateTime            = lastTime;
    testProgram.window.onClose = [&closing](auto&) { closing = true; };
    while (true) {
        testProgram.PollEvents();
        if (closing)
            break;
        const auto now   = std::chrono::high_resolution_clock::now();
        const auto delta = std::chrono::duration<double, std::milli>(now - lastTime).count();
        lastTime         = now;
        fpsCounter.StartFrame();
        auto updateDelta = std::chrono::duration<double, std::milli>(now - updateTime).count();
        if (updateDelta >= 15) {
            updateTime = now;
            camera.Update();
            Renderer::Update(renderer);
        }
        Renderer::Render(renderer);
        Renderer::SwapChain::Wait(swapChain);
        Renderer::SwapChain::Present(swapChain, renderBuffer);
        fpsCounter.EndFrame();
        if (std::chrono::duration<double, std::milli>(now - printTime).count() >= 48) {
            printTime = now;
            fpsCounter.Print();
        }
    }
    Renderer::Unload(renderer, *scene);
    return 0;
}
