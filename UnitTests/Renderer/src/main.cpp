#include <ECS/Registry.hpp>

#include <SG/Entity/Node/Node.hpp>
#include <SG/Entity/Camera.hpp>
#include <SG/Scene/Scene.hpp>
#include <SG/ShapeGenerator/Cube.hpp>

#include <Renderer/Renderer.hpp>
#include <Renderer/RenderBuffer.hpp>
#include <Renderer/SwapChain.hpp>

#include <Tools/ScopedTimer.hpp>

#include <Windows.h>

#include <functional>

using namespace TabGraph;

struct Window {
    Window(const Renderer::Handle& a_Renderer, uint32_t a_Width, uint32_t a_Height, bool a_VSync = true);
    void ResizeCallback(const uint32_t a_Width, const uint32_t a_Height) {
        if (a_Width == 0 || a_Height == 0 || closing) return;
        Renderer::CreateSwapChainInfo swapChainInfo;
        swapChainInfo.vSync = vSync;
        swapChainInfo.hwnd = hwnd;
        swapChainInfo.width = width = a_Width;
        swapChainInfo.height = height = a_Height;
        swapChainInfo.imageCount = 3;
        if (swapChain != nullptr)
            swapChain = Renderer::SwapChain::Recreate(swapChain, swapChainInfo);
        else swapChain = Renderer::SwapChain::Create(renderer, swapChainInfo);
    }
    void Present(const Renderer::RenderBuffer::Handle& a_RenderBuffer) {
        Renderer::SwapChain::Present(swapChain, a_RenderBuffer);
    }

    void ClosingCallback() {
        closing = true;
    }
    void PushEvents() {
        MSG msg{ 0 };
        while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
            DispatchMessage(&msg);
    }
    void Show() {
        ShowWindow(hwnd, SW_SHOWDEFAULT);
        Update();
    }
    void Update() {
        UpdateWindow(hwnd);
    }

    HWND hwnd;
    bool vSync{ true };
    bool closing{ false };
    uint32_t width, height;
    Renderer::Handle renderer;
    Renderer::SwapChain::Handle swapChain;
    std::function<void(Window&)> OnClose;
    std::function<void(Window&)> OnPaint;
    std::function<void(Window&, uint32_t, uint32_t)> OnMaximize;
    std::function<void(Window&, uint32_t, uint32_t)> OnMinimize;
    std::function<void(Window&, uint32_t, uint32_t)> OnRestore;
    std::function<void(Window&, uint32_t, uint32_t)> OnResize;
};

LRESULT CALLBACK Wndproc(
    HWND hwnd,        // handle to window
    UINT uMsg,        // message identifier
    WPARAM wParam,    // first message parameter
    LPARAM lParam)    // second message parameter
{
    const auto window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (window == nullptr) return DefWindowProc(hwnd, uMsg, wParam, lParam);
    switch (uMsg)
    {
    case WM_PAINT:
        if (window->OnPaint) window->OnPaint(*window);
        break;
    case WM_SIZE:
        switch (wParam) {
        case SIZE_MAXIMIZED:
            window->ResizeCallback(LOWORD(lParam), HIWORD(lParam));
            if (window->OnMaximize) window->OnMaximize(*window, LOWORD(lParam), HIWORD(lParam));
            break;
        case SIZE_MINIMIZED:
            if (window->OnMinimize) window->OnMinimize(*window, LOWORD(lParam), HIWORD(lParam));
            break;
        case SIZE_RESTORED:
            window->ResizeCallback(LOWORD(lParam), HIWORD(lParam));
            if (window->OnRestore) window->OnRestore(*window, LOWORD(lParam), HIWORD(lParam));
            break;
        default:
            window->ResizeCallback(LOWORD(lParam), HIWORD(lParam));
            if (window->OnResize) window->OnResize(*window, LOWORD(lParam), HIWORD(lParam));
        }
        break;
    case WM_CLOSE:
        window->ClosingCallback();
        if (window->OnClose) window->OnClose(*window);
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

struct WindowClass : WNDCLASSEX
{
    WindowClass()
    {
        std::memset(this, 0, sizeof(WindowClass));
        cbSize = sizeof(WindowClass);
        style = CS_HREDRAW | CS_VREDRAW;
        lpfnWndProc = Wndproc;
        hInstance = GetModuleHandle(0);
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
    hwnd = CreateWindowEx(
        0,
        wndclass.lpszClassName,
        "TabGraph::UnitTests::Renderer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        nullptr, nullptr, wndclass.hInstance, nullptr);
    MSG msg{ 0 };
    while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
        DispatchMessage(&msg);
        if (msg.message == WM_CREATE) break;
    }
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
}

constexpr auto testWindowWidth = 1280;
constexpr auto testWindowHeight = 800;

int main(int argc, char const *argv[])
{
    auto registry = ECS::DefaultRegistry::Create();
    auto renderer = Renderer::Create({ "UnitTest", 100 });
    auto window = Window(renderer, testWindowWidth, testWindowHeight, false);
    auto renderBuffer = Renderer::RenderBuffer::Create(renderer, { window.width, window.height });

    //build a test scene
    SG::Scene testScene(registry, "testScene");
    {
        auto testCube = SG::Cube::CreateMesh("testCube", { 1, 1, 1 });
        for (auto i = 0u; i < 10; ++i) {
            auto testEntity = SG::Node::Create(registry);
            testEntity.AddComponent<SG::Component::Mesh>(testCube);
            testEntity.GetComponent<SG::Component::Transform>().position.x = i;
            testScene.AddEntity(testEntity);
        }

        auto testCamera = SG::Camera::Create(registry);
        testCamera.GetComponent<SG::Component::Transform>().position = { 5, 5, 5 };
        SG::Node::LookAt(testCamera, glm::vec3(0));
        testScene.AddEntity(testCamera);
        testScene.SetCamera(testCamera);
    }
    {
        Tools::ScopedTimer timer("Loading Test Scene");
        Renderer::Load(renderer, testScene);
    }

    window.Show();
    while (true) {
        window.PushEvents();
        if (window.closing) break;
        Renderer::Render(renderer, testScene, renderBuffer);
        window.Present(renderBuffer);
        Renderer::Update(renderer);
    }
    return 0;
}