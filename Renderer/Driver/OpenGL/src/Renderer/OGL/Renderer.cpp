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

TabGraph::Renderer::Impl::Impl(const CreateRendererInfo& a_Info)
    : name(a_Info.name)
    , version(a_Info.applicationVersion)
{
#ifdef _WIN32
    static uint32_t s_ClassId = 0;
    OpenGL::Win32::Initialize();
    windowClassName = "DummyWindow" + std::to_string(s_ClassId++);
    WNDCLASSEX wndclass {};
    std::memset(&wndclass, 0, sizeof(wndclass));
    wndclass.cbSize        = sizeof(wndclass);
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = DefWindowProc;
    wndclass.hInstance     = GetModuleHandle(nullptr);
    wndclass.lpszClassName = windowClassName.c_str();
    WIN32_CHECK_ERROR(RegisterClassEx(&wndclass));
    window         = TabGraph::Window::Win32::Create(windowClassName, windowClassName);
    displayContext = GetDC(HWND(window));
    OpenGL::Win32::SetDefaultPixelFormat(displayContext);
    renderContext = OpenGL::Win32::CreateContext(displayContext);
    PushRenderCommand(
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
    PushRenderCommand(
        [this] {
            WIN32_CHECK_ERROR(wglMakeCurrent(HDC(displayContext), HGLRC(renderContext)));
        },
        true);
    WIN32_CHECK_ERROR(wglDeleteContext(HGLRC(renderContext)));
    WIN32_CHECK_ERROR(ReleaseDC(HWND(window), HDC(displayContext)));
    DestroyWindow(HWND(window));
    UnregisterClass(windowClassName.c_str(), GetModuleHandle(0));
#endif
}
}
