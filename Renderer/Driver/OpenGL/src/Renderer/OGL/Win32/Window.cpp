#include <Renderer/OGL/Win32/Error.hpp>

#include <GL/glew.h>
#include <GL/wglew.h>

#include <string>
#include <windows.h>

namespace TabGraph::Window::Win32 {
void* Create(const std::string& a_ClassName, const std::string& a_Name)
{
    const auto hwnd = CreateWindowEx(
        0,
        a_ClassName.c_str(),
        a_Name.c_str(),
        0,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
    WIN32_CHECK_ERROR(hwnd != nullptr);
    MSG msg = { 0 };
    while (PeekMessage(&msg, hwnd, 0, 0, PM_NOYIELD | PM_REMOVE)) {
        DispatchMessage(&msg);
        if (msg.message == WM_CREATE)
            break;
    }
    return hwnd;
}
}
