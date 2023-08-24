#include <Renderer/OGL/Renderer.hpp>
#include <Renderer/Structs.hpp>
#include <SG/Scene/Scene.hpp>

#ifdef _WIN32
#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/OpenGL.hpp>
#include <Renderer/OGL/Win32/Window.hpp>
#endif

#include <GL/wglew.h>

#include <stdexcept>

namespace TabGraph::Renderer {
Handle Create(const CreateRendererInfo& a_Info)
{
    return Handle(new Impl(a_Info));
}

void RegisterWindowClass()
{
    auto moduleHandle = GetModuleHandle(nullptr);
    WNDCLASS wndclass {};
    if (GetClassInfo(moduleHandle, "DummyWindow", &wndclass))
        return; // the window class is already registered
    std::string windowClassName = "DummyWindow";
    std::memset(&wndclass, 0, sizeof(wndclass));
    // wndclass.cbSize        = sizeof(wndclass);
    wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.lpfnWndProc   = DefWindowProc;
    wndclass.hInstance     = GetModuleHandle(nullptr);
    wndclass.lpszClassName = windowClassName.c_str();
    WIN32_CHECK_ERROR(RegisterClass(&wndclass));
}

TabGraph::Renderer::Impl::Impl(const CreateRendererInfo& a_Info)
    : name(a_Info.name)
    , version(a_Info.applicationVersion)
{
#ifdef _WIN32
    OpenGL::Win32::Initialize();
    RegisterWindowClass();
    window         = TabGraph::Window::Win32::Create("DummyWindow", "DummyWindow");
    displayContext = GetDC(HWND(window));
    OpenGL::Win32::SetDefaultPixelFormat(displayContext);
    renderContext = OpenGL::Win32::CreateContext(displayContext);
    PushRenderCmd(
        [this]() {
            WIN32_CHECK_ERROR(wglMakeCurrent(HDC(displayContext), HGLRC(renderContext)));
        },
        false);
#endif
}

void Load(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene)
{
}

void Load(
    const Handle& a_Renderer,
    const uint32_t a_EntityID)
{
}

void Unload(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene)
{
}

void Unload(
    const Handle& a_Renderer,
    const uint32_t a_EntityID)
{
}

void Render(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene,
    const RenderBuffer::Handle& a_Buffer)
{
}

void Update(const Handle& a_Renderer)
{
}

Impl::~Impl()
{
#ifdef _WIN32
    PushRenderCmd(
        [this] {
            WIN32_CHECK_ERROR(wglMakeCurrent(HDC(displayContext), nullptr));
        },
        true);
    WIN32_CHECK_ERROR(wglDeleteContext(HGLRC(renderContext)));
    WIN32_CHECK_ERROR(ReleaseDC(HWND(window), HDC(displayContext)));
    DestroyWindow(HWND(window));
    UnregisterClass(windowClassName.c_str(), GetModuleHandle(0));
#endif
}
}
