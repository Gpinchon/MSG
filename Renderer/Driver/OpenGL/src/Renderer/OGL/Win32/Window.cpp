#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/Window.hpp>

#include <GL/glew.h>
#include <GL/wglew.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <windows.h>

namespace TabGraph::Renderer::RAII {
std::unordered_map<std::string, unsigned> s_RegisteredClassesRefs;

void RegisterWindowClass(const std::string& a_ClassName)
{
    auto& refCount = s_RegisteredClassesRefs[a_ClassName];
    if (refCount > 0) {
        ++refCount;
        return;
    }
    auto moduleHandle = GetModuleHandle(nullptr);
    WNDCLASS wndclass {};
    std::memset(&wndclass, 0, sizeof(wndclass));
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = DefWindowProc;
    wndclass.hInstance     = GetModuleHandle(nullptr);
    wndclass.lpszClassName = a_ClassName.c_str();
    WIN32_CHECK_ERROR(RegisterClass(&wndclass));
}

void UnregisterWindowClass(const std::string& a_ClassName)
{
    auto& refCount = s_RegisteredClassesRefs[a_ClassName];
    --refCount;
    if (refCount == 0)
        UnregisterClass(a_ClassName.c_str(), GetModuleHandle(0));
}

void* CreateHWND(const std::string& a_ClassName, const std::string& a_Name)
{
    RegisterWindowClass(a_ClassName);
    const auto hwnd = CreateWindow(
        a_ClassName.c_str(),
        a_Name.c_str(),
        0, 0, 0, 0, 0,
        HWND(nullptr),
        HMENU(nullptr),
        GetModuleHandle(nullptr),
        nullptr);
    WIN32_CHECK_ERROR(hwnd != nullptr);
    MSG msg = { 0 };
    while (PeekMessage(&msg, hwnd, 0, 0, PM_NOYIELD | PM_REMOVE)) {
        DispatchMessage(&msg);
        if (msg.message == WM_CREATE)
            break;
    }
    return hwnd;
}

void DestroyHWND(const void* a_HWND)
{
    char className[4096];
    std::memset(className, 0, sizeof(className));
    GetClassName(HWND(a_HWND), className, sizeof(className));
    UnregisterWindowClass(className);
}

Window::Window(const std::string& a_ClassName, const std::string& a_Name)
    : windowClass(a_ClassName)
    , hwnd(CreateHWND(a_ClassName, a_Name))
{
}

Window::~Window()
{
    DestroyHWND(hwnd);
}
}
