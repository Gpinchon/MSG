#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/Window.hpp>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <windows.h>

#include <stdexcept>

namespace TabGraph::OpenGL::Win32 {
void Initialize()
{
    static bool s_Initialized = false;
    if (s_Initialized)
        return; // we only need to initialize OGL once for the whole execution
    WNDCLASSEX wndclass {};
    std::memset(&wndclass, 0, sizeof(wndclass));
    wndclass.cbSize        = sizeof(wndclass);
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = DefWindowProc;
    wndclass.hInstance     = GetModuleHandle(0);
    wndclass.lpszClassName = "OpenGL::Initialize";
    WIN32_CHECK_ERROR(RegisterClassEx(&wndclass));
    const auto tempHWND = HWND(Window::Win32::Create("OpenGL::Initialize", "OpenGL::Initialize"));
    const auto tempDC   = GetDC(tempHWND);
    {
        PIXELFORMATDESCRIPTOR pfd;
        std::memset(&pfd, 0, sizeof(pfd));
        pfd.nSize          = sizeof(pfd);
        pfd.nVersion       = 1;
        pfd.dwFlags        = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType     = PFD_TYPE_RGBA;
        pfd.iLayerType     = PFD_MAIN_PLANE;
        pfd.cColorBits     = 32;
        const auto pformat = ChoosePixelFormat(tempDC, &pfd);
        WIN32_CHECK_ERROR(pformat != 0);
        WIN32_CHECK_ERROR(SetPixelFormat(tempDC, pformat, nullptr));
    }
    const auto tempHGLRC = wglCreateContext(tempDC);
    glewExperimental     = true;
    WIN32_CHECK_ERROR(tempHGLRC != nullptr);
    WIN32_CHECK_ERROR(wglMakeCurrent(tempDC, tempHGLRC));
    WIN32_CHECK_ERROR(glewInit() == GLEW_OK); // load OGL extensions
    WIN32_CHECK_ERROR(wglewInit() == GLEW_OK); // load WGL extensions
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(tempHGLRC);
    ReleaseDC(tempHWND, tempDC);
    DestroyWindow(tempHWND);
    UnregisterClass("OpenGL::Initialize", GetModuleHandle(0));
    s_Initialized = true; // OGL was initialized, no need to do it again next time
}

void* CreateContext(const void* a_HDC)
{
    if (!WGLEW_ARB_create_context)
        throw std::runtime_error("Modern context creation not supported !");
    if (!WGLEW_ARB_create_context_robustness)
        throw std::runtime_error("Robust context creation not supported !");
    const int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef _DEBUG
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
#else
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB,
#endif _DEBUG
        0
    };
    auto hglrc = wglCreateContextAttribsARB(HDC(a_HDC), 0, attribs);
    WIN32_CHECK_ERROR(hglrc != nullptr);
    return hglrc;
}

void SetDefaultPixelFormat(const void* a_HDC)
{
    const auto hdc          = HDC(a_HDC);
    const int attribIList[] = {
        WGL_DRAW_TO_PBUFFER_ARB, true,
        WGL_SUPPORT_OPENGL_ARB, true,
        WGL_DOUBLE_BUFFER_ARB, false,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLORSPACE_EXT, WGL_COLORSPACE_SRGB_EXT,
        WGL_COLOR_BITS_ARB, 0,
        0
    };
    int32_t pixelFormat     = 0;
    uint32_t pixelFormatNbr = 0;
    WIN32_CHECK_ERROR(wglChoosePixelFormatARB(hdc, attribIList, nullptr, 1, &pixelFormat, &pixelFormatNbr));
    WIN32_CHECK_ERROR(pixelFormat != 0);
    WIN32_CHECK_ERROR(pixelFormatNbr != 0);
    WIN32_CHECK_ERROR(SetPixelFormat(hdc, pixelFormat, nullptr));
}
}
