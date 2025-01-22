#include <MSG/Renderer/OGL/Win32/Platform.hpp>

#include <iostream>
#include <stdexcept>
#include <unordered_map>

#include <windows.h>

std::unordered_map<std::string, unsigned> s_RegisteredClassesRefs;

void Platform::RegisterWindowClass(const std::string& a_ClassName)
{
    auto& refCount = s_RegisteredClassesRefs[a_ClassName];
    if (refCount > 0) {
        ++refCount;
        return;
    }
    auto moduleHandle = GetModuleHandle(nullptr);
    WNDCLASS wndclass {};
    std::memset(&wndclass, 0, sizeof(wndclass));
    wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_CLASSDC;
    wndclass.lpfnWndProc   = DefWindowProc;
    wndclass.hInstance     = GetModuleHandle(nullptr);
    wndclass.lpszClassName = a_ClassName.c_str();
    WIN32_CHECK_ERROR(RegisterClass(&wndclass));
}

void Platform::UnregisterWindowClass(const std::string& a_ClassName)
{
    auto& refCount = s_RegisteredClassesRefs[a_ClassName];
    --refCount;
    if (refCount == 0)
        UnregisterClass(a_ClassName.c_str(), GetModuleHandle(0));
}

std::any Platform::CreateHWND(const std::string& a_ClassName, const std::string& a_Name)
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

void Platform::DestroyHWND(const std::any a_HWND)
{
    char className[4096];
    std::memset(className, 0, sizeof(className));
    auto hwnd = std::any_cast<HWND>(a_HWND);
    GetClassName(hwnd, className, sizeof(className));
    UnregisterWindowClass(className);
    DestroyWindow(hwnd);
}

std::any Platform::GetDC(std::any a_HWND)
{
    auto hdc = GetDC(std::any_cast<HWND>(a_HWND));
    WIN32_CHECK_ERROR(hdc != nullptr);
    return hdc;
}

void Platform::ReleaseDC(std::any a_HWND, std::any a_DC)
{
    ReleaseDC(std::any_cast<HWND>(a_HWND), std::any_cast<HDC>(a_DC));
}

void Platform::CheckError(const std::string& a_File, const uint32_t& a_Line, const std::string& a_Procedure)
{
    const auto errorCode = GetLastError();
    SetLastError(ERROR_SUCCESS);
    std::string errorString = a_File + "@" + std::to_string(a_Line) + " : " + a_Procedure + " failed, **ERROR CODE** : " + std::to_string(errorCode);
    std::cerr << errorString << std::endl;
    throw std::runtime_error(errorString);
}

int32_t Platform::GetDefaultPixelFormat(std::any a_DC)
{
    PIXELFORMATDESCRIPTOR pfd;
    std::memset(&pfd, 0, sizeof(pfd));
    pfd.nSize          = sizeof(pfd);
    pfd.nVersion       = 1;
    pfd.dwFlags        = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType     = PFD_TYPE_RGBA;
    pfd.iLayerType     = PFD_MAIN_PLANE;
    pfd.cColorBits     = 32;
    const auto pformat = ChoosePixelFormat(std::any_cast<HDC>(a_DC), &pfd);
    WIN32_CHECK_ERROR(pformat != 0);
    return pformat;
}

void Platform::SetPixelFormat(std::any a_HDC, const int32_t& a_PixelFormat)
{
    WIN32_CHECK_ERROR(SetPixelFormat(std::any_cast<HDC>(a_HDC), a_PixelFormat, nullptr));
}
