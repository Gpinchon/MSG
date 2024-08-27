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
#include <SG/Core/Texture/Sampler.hpp>
#include <SG/Core/Texture/Texture.hpp>
#include <SG/Entity/Camera.hpp>
#include <SG/Entity/Light/PunctualLight.hpp>
#include <SG/Entity/Node.hpp>
#include <SG/Scene/Scene.hpp>
#include <SG/ShapeGenerator/Cube.hpp>
#include <Tools/FPSCounter.hpp>
#include <Tools/ScopedTimer.hpp>
#ifdef __linux
#define SDL_VIDEO_DRIVER_X11
#endif //__linux
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_syswm.h>
#include <filesystem>
#include <functional>

using namespace TabGraph;

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
        do {
            SDL_WaitEvent(&event);
        } while (event.type != SDL_WINDOWEVENT && event.window.event != SDL_WINDOWEVENT_EXPOSED);
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

struct Mouse {
    std::function<void(const SDL_MouseButtonEvent&)> onButton;
    std::function<void(const SDL_MouseMotionEvent&)> onMotion;
    std::function<void(const SDL_MouseWheelEvent&)> onWheel;
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
            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.windowID == window.GetID())
                    window.ProcessEvent(event.window);
                break;
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.windowID == window.GetID() && mouse.onButton)
                    mouse.onButton(event.button);
                break;
            case SDL_MOUSEMOTION:
                if (event.motion.windowID == window.GetID() && mouse.onMotion)
                    mouse.onMotion(event.motion);
                break;
            case SDL_MOUSEWHEEL:
                if (event.wheel.windowID == window.GetID() && mouse.onWheel)
                    mouse.onWheel(event.wheel);
                break;
            default:
                break;
            }
        }
    }

private:
    int _initialized = SDL_Init(SDL_INIT_EVERYTHING);

public:
    Test::Window window;
    Test::Mouse mouse;
};
}

constexpr auto testWindowWidth  = 1280;
constexpr auto testWindowHeight = 720;

struct Args {
    Args(const int& argc, char const* argv[])
    {
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "--env") {
                i++;
                envPath = argv[i];
            } else if (arg == "--model") {
                i++;
                modelPath = argv[i];
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
    swapChainInfo.windowInfo.hwnd = a_WMInfo.info.win.window;
#elif defined __linux
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
    auto args = Args(argc, argv);
    if (args.envPath.empty() || args.modelPath.empty())
        return -1;
    std::cout << "envPath   " << args.envPath << std::endl;
    std::cout << "modelPath " << args.modelPath << std::endl;
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
                auto cubemap = std::make_shared<SG::Cubemap>(
                    parsedImage->GetPixelDescription(),
                    512, 512, *std::static_pointer_cast<SG::Image2D>(parsedImage));
                SG::TextureSampler skybox;
                skybox.texture      = std::make_shared<SG::Texture>(SG::TextureType::TextureCubemap, cubemap);
                skybox.sampler      = std::make_shared<SG::Sampler>();
                skybox.texture->GenerateMipmaps();
                auto lightIBLEntity = SG::PunctualLight::Create(registry);
                auto& lightIBLComp  = lightIBLEntity.GetComponent<SG::Component::PunctualLight>();
                SG::Component::LightIBL lightIBLData({ 64, 64 }, skybox.texture);
                lightIBLData.intensity = 1;
                lightIBLComp           = lightIBLData;
               
                scene->AddEntity(lightIBLEntity);
                scene->SetSkybox(skybox);
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
    rendererSettings.enableTAA = true;

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
    int lastMouseX             = -1;
    int lastMouseY             = -1;
    testProgram.mouse.onMotion = [&camera, &lastMouseX, &lastMouseY](const SDL_MouseMotionEvent& a_Event) {
        auto buttons = SDL_GetMouseState(nullptr, nullptr);
        if (lastMouseX == -1)
            lastMouseX = a_Event.x;
        if (lastMouseY == -1)
            lastMouseY = a_Event.y;
        auto relMoveX = lastMouseX - a_Event.x;
        auto relMoveY = lastMouseY - a_Event.y;
        if ((buttons & SDL_BUTTON_LMASK) != 0) {
            camera.theta += relMoveY * 0.001f;
            camera.phi += relMoveX * 0.001f;
        }
        if ((buttons & SDL_BUTTON_RMASK) != 0) {
            auto& targetTransform = camera.targetEntity.GetComponent<SG::Component::Transform>();
            auto cameraRight      = SG::Node::GetRight(camera.cameraEntity) * (relMoveX * 0.001f);
            auto cameraUp         = SG::Node::GetUp(camera.cameraEntity) * -(relMoveY * 0.001f);
            targetTransform.SetPosition(targetTransform.position + cameraRight + cameraUp);
        }
        lastMouseX = a_Event.x;
        lastMouseY = a_Event.y;
    };
    testProgram.mouse.onWheel = [&camera](const SDL_MouseWheelEvent& a_Event) {
        camera.radius -= a_Event.y * 0.05f;
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
