#include <ECS/Registry.hpp>

#include <SG/Core/Material.hpp>
#include <SG/Core/Material/Extension/SpecularGlossiness.hpp>
#include <SG/Entity/Camera.hpp>
#include <SG/Entity/Light/PunctualLight.hpp>
#include <SG/Entity/Node.hpp>
#include <SG/Scene/Scene.hpp>
#include <SG/ShapeGenerator/Cube.hpp>

#include <Renderer/RenderBuffer.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/SwapChain.hpp>

#include <Tools/FPSCounter.hpp>
#include <Tools/ScopedTimer.hpp>

using namespace TabGraph;

#ifdef WIN32
#include <Windows.h>
#include <functional>

struct Window {
    Window(const Renderer::Handle& a_Renderer, uint32_t a_Width, uint32_t a_Height, bool a_VSync = true);
    void ResizeCallback(const uint32_t a_Width, const uint32_t a_Height)
    {
        if (a_Width == 0 || a_Height == 0 || closing)
            return;
        Renderer::CreateSwapChainInfo swapChainInfo;
        swapChainInfo.vSync = vSync;
        swapChainInfo.hwnd  = hwnd;
        swapChainInfo.width = width = a_Width;
        swapChainInfo.height = height = a_Height;
        swapChainInfo.imageCount      = 3;
        if (swapChain != nullptr)
            swapChain = Renderer::SwapChain::Recreate(swapChain, swapChainInfo);
        else
            swapChain = Renderer::SwapChain::Create(renderer, swapChainInfo);
    }
    void Present(const Renderer::RenderBuffer::Handle& a_RenderBuffer)
    {
        Renderer::SwapChain::Present(swapChain, a_RenderBuffer);
    }

    void ClosingCallback()
    {
        closing = true;
    }
    void PushEvents()
    {
        MSG msg { 0 };
        while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
            DispatchMessage(&msg);
    }
    void Show()
    {
        ShowWindow(hwnd, SW_SHOWDEFAULT);
        Update();
    }
    void Update()
    {
        UpdateWindow(hwnd);
    }

    HWND hwnd;
    bool vSync { true };
    bool closing { false };
    uint32_t width, height;
    Renderer::Handle renderer;
    Renderer::SwapChain::Handle swapChain;
    std::function<void(Window&)> OnClose;
    std::function<void(Window&)> OnPaint;
    std::function<void(Window&, uint32_t, uint32_t)> OnResize; // Will be called first of any event that resize the window
    std::function<void(Window&, uint32_t, uint32_t)> OnMaximize;
    std::function<void(Window&, uint32_t, uint32_t)> OnMinimize;
    std::function<void(Window&, uint32_t, uint32_t)> OnRestore;
};

LRESULT CALLBACK Wndproc(
    HWND hwnd, // handle to window
    UINT uMsg, // message identifier
    WPARAM wParam, // first message parameter
    LPARAM lParam) // second message parameter
{
    const auto window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (window == nullptr)
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    switch (uMsg) {
    case WM_PAINT:
        if (window->OnPaint)
            window->OnPaint(*window);
        break;
    case WM_SIZE:
        window->ResizeCallback(LOWORD(lParam), HIWORD(lParam));
        if (window->OnResize)
            window->OnResize(*window, LOWORD(lParam), HIWORD(lParam));
        switch (wParam) {
        case SIZE_MAXIMIZED:
            if (window->OnMaximize)
                window->OnMaximize(*window, LOWORD(lParam), HIWORD(lParam));
            break;
        case SIZE_MINIMIZED:
            if (window->OnMinimize)
                window->OnMinimize(*window, LOWORD(lParam), HIWORD(lParam));
            break;
        case SIZE_RESTORED:
            if (window->OnRestore)
                window->OnRestore(*window, LOWORD(lParam), HIWORD(lParam));
            break;
        }
        break;
    case WM_CLOSE:
        window->ClosingCallback();
        if (window->OnClose)
            window->OnClose(*window);
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

struct WindowClass : WNDCLASSEX {
    WindowClass()
    {
        std::memset(this, 0, sizeof(WindowClass));
        cbSize        = sizeof(WindowClass);
        style         = CS_HREDRAW | CS_VREDRAW;
        lpfnWndProc   = Wndproc;
        hInstance     = GetModuleHandle(0);
        lpszClassName = "TabGraph::Renderer::Window";
        if (!RegisterClassEx(this)) {
            std::cerr << "Error in Function = " << __FUNCTION__ << " at line = " << __LINE__ << ", with error code = " << GetLastError() << std::endl;
            throw std::runtime_error("Could not register window class");
        }
    }
    ~WindowClass()
    {
        UnregisterClass(lpszClassName, hInstance);
    }
};

static inline auto& GetWindowClass()
{
    static std::mutex s_Mutex;
    std::lock_guard lock(s_Mutex);
    static WindowClass s_Wndclass;
    return s_Wndclass;
}

Window::Window(const Renderer::Handle& a_Renderer, uint32_t a_Width, uint32_t a_Height, bool a_VSync)
    : renderer(a_Renderer)
    , vSync(a_VSync)
    , width(a_Width)
    , height(a_Height)
{
    const auto& wndclass = GetWindowClass();
    hwnd                 = CreateWindowEx(
        0,
        wndclass.lpszClassName,
        "TabGraph::UnitTests::Renderer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        nullptr, nullptr, wndclass.hInstance, nullptr);
    MSG msg { 0 };
    while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
        DispatchMessage(&msg);
        if (msg.message == WM_CREATE)
            break;
    }
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
}
#elif defined __linux__

#include <X11/Xlib.h>

struct TabGraphWindow {
    TabGraphWindow(const Renderer::Handle& a_Renderer, Display* a_X11Display, uint32_t a_Width, uint32_t a_Height, bool a_VSync = true)
        : renderer(a_Renderer)
        , display(a_X11Display)
        , vSync(a_VSync)
        , width(a_Width)
        , height(a_Height)
    {
        if (display == nullptr) {
            std::cerr << "Cannot open display" << std::endl;
            exit(1);
        }
        auto screen      = DefaultScreen(display);
        auto parent      = RootWindow(display, screen);
        auto border      = BlackPixel(display, screen);
        auto background  = WhitePixel(display, screen);
        auto borderWidth = 1;
        auto x           = 0;
        auto y           = 0;
        window           = XCreateSimpleWindow(
            display, parent,
            x, y, a_Width, a_Height,
            borderWidth,
            border, background);
        XSelectInput(display, window, ExposureMask | KeyPressMask);
        ResizeCallback(width, height);
    }
    void ResizeCallback(const uint32_t a_Width, const uint32_t a_Height)
    {
        if (a_Width == 0 || a_Height == 0 || closing)
            return;
        Renderer::CreateSwapChainInfo swapChainInfo;
        swapChainInfo.vSync              = vSync;
        swapChainInfo.windowInfo.display = display;
        swapChainInfo.windowInfo.window  = window;
        swapChainInfo.width = width = a_Width;
        swapChainInfo.height = height = a_Height;
        swapChainInfo.imageCount      = 3;
        if (swapChain != nullptr)
            swapChain = Renderer::SwapChain::Recreate(swapChain, swapChainInfo);
        else
            swapChain = Renderer::SwapChain::Create(renderer, swapChainInfo);
    }
    ~TabGraphWindow()
    {
        XCloseDisplay(display);
    }
    void Show()
    {
        XMapWindow(display, window);
    }
    void PushEvents()
    {
        while (XPending(display) > 0) {
            XEvent event;
            XNextEvent(display, &event);
            switch (event.type) {
            case DestroyNotify:
                closing = true;
                break;
            case ResizeRequest:
                ResizeCallback(event.xresizerequest.width, event.xresizerequest.height);
                if (OnResize != nullptr)
                    OnResize(*this, event.xresizerequest.width, event.xresizerequest.height);
                break;
            case Expose:
                if (event.xexpose.count > 0)
                    break; // Only handle last expose
                ResizeCallback(event.xexpose.width, event.xexpose.height);
                if (OnResize != nullptr)
                    OnResize(*this, event.xexpose.width, event.xexpose.height);

                break;
            default:
                break;
            }
        }
    }
    void Present(const Renderer::RenderBuffer::Handle& a_RenderBuffer)
    {
        Renderer::SwapChain::Wait(swapChain);
        Renderer::SwapChain::Present(swapChain, a_RenderBuffer);
    }
    Display* display;
    Window window;
    bool vSync      = false;
    bool closing    = false;
    uint32_t width  = 0;
    uint32_t height = 0;
    Renderer::Handle renderer;
    Renderer::SwapChain::Handle swapChain;
    std::function<void(TabGraphWindow&)> OnClose;
    std::function<void(TabGraphWindow&)> OnPaint;
    std::function<void(TabGraphWindow&, uint32_t, uint32_t)> OnResize; // Will be called first of any event that resize the window
    std::function<void(TabGraphWindow&, uint32_t, uint32_t)> OnMaximize;
    std::function<void(TabGraphWindow&, uint32_t, uint32_t)> OnMinimize;
    std::function<void(TabGraphWindow&, uint32_t, uint32_t)> OnRestore;
};
#endif

constexpr auto testWindowWidth  = 1280;
constexpr auto testWindowHeight = 800;
constexpr auto testGridSize     = 20;
constexpr auto testCubesNbr     = 10;
constexpr auto testLightNbr     = 10;

auto GetCameraProj(const uint& a_Width, const uint& a_Height)
{
    SG::Component::Projection::PerspectiveInfinite cameraProj;
    cameraProj.znear = 1.f;
    // cameraProj.zfar        = 1000.f;
    cameraProj.fov         = 90.f;
    cameraProj.aspectRatio = a_Width / float(a_Height);
    return cameraProj;
}

int main(int argc, char const* argv[])
{
    auto display      = XOpenDisplay(nullptr);
    auto registry     = ECS::DefaultRegistry::Create();
    auto renderer     = Renderer::Create({ .name = "UnitTest", .applicationVersion = 100, .display = display }, { .mode = Renderer::RendererMode::Forward });
    auto window       = TabGraphWindow(renderer, display, testWindowWidth, testWindowHeight, false);
    auto renderBuffer = Renderer::RenderBuffer::Create(renderer, { window.width, window.height });

    // build a test scene
    SG::Scene testScene(registry, "testScene");
    testScene.SetBackgroundColor({ 0.529, 0.808, 0.922 });

    auto testCamera                                             = SG::Camera::Create(registry);
    testCamera.GetComponent<SG::Component::Camera>().projection = GetCameraProj(testWindowWidth, testWindowHeight);
    testCamera.GetComponent<SG::Component::Transform>().SetPosition({ 5, 5, 2 });
    SG::Node::LookAt(testCamera, glm::vec3(0));
    testScene.AddEntity(testCamera);
    testScene.SetCamera(testCamera);
    std::vector<ECS::DefaultRegistry::EntityRefType> testEntitis;
    {
        auto testCube = SG::Cube::CreateMesh("testCube", { 1, 1, 1 });
        SG::SpecularGlossinessExtension specGloss;
        // specGloss.glossinessFactor = 0;
        testCube.GetMaterials().front()->AddExtension(specGloss);
        for (auto x = 0u; x < testCubesNbr; ++x) {
            float xCoord = (x / float(testCubesNbr) - 0.5) * testGridSize;
            for (auto y = 0u; y < testCubesNbr; ++y) {
                float yCoord    = (y / float(testCubesNbr) - 0.5) * testGridSize;
                auto testEntity = SG::Node::Create(registry);
                testEntitis.push_back(testEntity);
                testEntity.AddComponent<SG::Component::Mesh>(testCube);
                testEntity.GetComponent<SG::Component::Transform>().SetPosition({ xCoord, yCoord, 0 });
                testScene.AddEntity(testEntity);
            }
        }
    }
    {
        unsigned currentLight = 0;
        for (auto x = 0u; x < testLightNbr; ++x) {
            float xCoord = (x / float(testLightNbr) - 0.5) * testGridSize;
            for (auto y = 0u; y < testLightNbr; ++y) {
                float yCoord         = (y / float(testLightNbr) - 0.5) * testGridSize;
                auto light           = SG::PunctualLight::Create(registry);
                auto& lightData      = light.GetComponent<SG::Component::PunctualLight>();
                auto& lightTransform = light.GetComponent<SG::Component::Transform>();
                lightTransform.SetPosition({ xCoord, yCoord, 1 });
                SG::Node::LookAt(light, { xCoord, yCoord, 0 });
                if (currentLight % 2 == 0) {
                    SG::Component::LightSpot spot;
                    spot.innerConeAngle = 0.3;
                    spot.outerConeAngle = 0.5;
                    lightData           = spot;
                } else {
                    lightData = SG::Component::LightPoint {};
                }
                std::visit([](auto& a_Data) {
                    a_Data.intensity = 1;
                    a_Data.range     = 1;
                    a_Data.color     = {
                        std::rand() / float(RAND_MAX),
                        std::rand() / float(RAND_MAX),
                        std::rand() / float(RAND_MAX)
                    };
                },
                    lightData);
                testScene.AddEntity(light);
                ++currentLight;
            }
        }
    }

    window.OnResize = [&renderer, &renderBuffer, testCamera](TabGraphWindow&, uint32_t a_Width, uint32_t a_Height) mutable {
        renderBuffer                                                         = Renderer::RenderBuffer::Create(renderer, { a_Width, a_Height });
        testCamera.template GetComponent<SG::Component::Camera>().projection = GetCameraProj(a_Width, a_Height);
        Renderer::SetActiveRenderBuffer(renderer, renderBuffer);
    };

    {
        Tools::ScopedTimer timer("Loading Test Scene");
        Renderer::Load(renderer, testScene);
    }

    Renderer::SetActiveScene(renderer, &testScene);
    Renderer::SetActiveRenderBuffer(renderer, renderBuffer);
    Renderer::Update(renderer);

    window.Show();
    FPSCounter fpsCounter;
    auto lastTime   = std::chrono::high_resolution_clock::now();
    auto printTime  = lastTime;
    auto updateTime = lastTime;
    while (true) {
        window.PushEvents();
        if (window.closing)
            break;
        const auto now   = std::chrono::high_resolution_clock::now();
        const auto delta = std::chrono::duration<double, std::milli>(now - lastTime).count();
        lastTime         = now;
        fpsCounter.StartFrame();
        auto updateDelta = std::chrono::duration<double, std::milli>(now - updateTime).count();
        if (updateDelta > 32) {
            for (auto& entity : testEntitis) {
                auto entityMaterial   = entity.GetComponent<SG::Component::Mesh>().GetMaterials().front();
                auto& entityTransform = entity.GetComponent<SG::Component::Transform>();
                auto& diffuseOffset   = entityMaterial->GetExtension<SG::SpecularGlossinessExtension>().diffuseTexture.transform.offset;
                diffuseOffset.x += 0.000005f * float(updateDelta);
                diffuseOffset.x = diffuseOffset.x > 2 ? 0 : diffuseOffset.x;
                auto rot        = entity.GetComponent<SG::Component::Transform>().rotation;
                rot             = glm::rotate(rot, 0.001f * float(updateDelta), { 0, 0, 1 });
                entityTransform.SetRotation(rot);
            }
            updateTime = now;
            Renderer::Update(renderer);
        }
        Renderer::Render(renderer);
        window.Present(renderBuffer);
        fpsCounter.EndFrame();
        if (std::chrono::duration<double, std::milli>(now - printTime).count() >= 48) {
            printTime = now;
            fpsCounter.Print();
        }
    }
    Renderer::Unload(renderer, testScene);
    return 0;
}
