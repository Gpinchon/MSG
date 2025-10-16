#include <MSG/OGLContext/Win32/Win32.hpp>

#include <iostream>
#include <stdexcept>
#include <unordered_map>

#include <windows.h>

std::unordered_map<std::string, std::weak_ptr<Win32::WNDClassWrapper>> s_RegisteredClasses;

auto GetWindowClass(std::string_view a_ClassName)
{
    auto& classWrapper = s_RegisteredClasses[std::string(a_ClassName)];
    if (!classWrapper.expired())
        return classWrapper.lock();
    auto newClassWrapper = std::make_shared<Win32::WNDClassWrapper>(a_ClassName);
    classWrapper         = newClassWrapper;
    return newClassWrapper;
}

std::shared_ptr<Win32::WNDClassWrapper> Win32::WNDClassWrapper::Create(std::string_view a_ClassName)
{
    return GetWindowClass(a_ClassName);
}

void Win32::RegisterWindowClass(const std::string& a_ClassName)
{
    auto hmodule = GetModuleHandle(nullptr);
    WNDCLASS wndclass {};
    std::memset(&wndclass, 0, sizeof(wndclass));
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = DefWindowProc;
    wndclass.hInstance     = hmodule;
    wndclass.lpszClassName = a_ClassName.c_str();
    WIN32_CHECK_ERROR(::RegisterClass(&wndclass));
}

void Win32::UnregisterWindowClass(const std::string& a_ClassName)
{
    auto hmodule = GetModuleHandle(nullptr);
    UnregisterClass(a_ClassName.c_str(), hmodule);
    if (s_RegisteredClasses.at(a_ClassName).expired())
        s_RegisteredClasses.erase(a_ClassName);
}

std::any Win32::CreateHWND(const CreateHWNDInfo& a_Info)
{
    const auto hwnd = CreateWindow(
        a_Info.className.c_str(),
        a_Info.name.c_str(),
        a_Info.style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        a_Info.width, a_Info.height,
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

std::any Win32::GetDesktopWindow()
{
    return ::GetDesktopWindow();
}

void Win32::DestroyHWND(const std::any a_HWND)
{
    auto hwnd = std::any_cast<HWND>(a_HWND);
    DestroyWindow(hwnd);
}

std::any Win32::GetDC(std::any a_HWND)
{
    auto hwnd = a_HWND.has_value() ? std::any_cast<HWND>(a_HWND) : nullptr;
    auto hdc  = ::GetDC(hwnd);
    WIN32_CHECK_ERROR(hdc != nullptr);
    return hdc;
}

std::any Win32::GetCompatibleDC(std::any a_HDC)
{
    auto hdc = a_HDC.has_value() ? std::any_cast<HDC>(a_HDC) : nullptr;
    return ::CreateCompatibleDC(hdc);
}

void Win32::ReleaseDC(std::any a_HWND, std::any a_HDC)
{
    auto hwnd = a_HWND.has_value() ? std::any_cast<HWND>(a_HWND) : nullptr;
    auto hdc  = a_HDC.has_value() ? std::any_cast<HDC>(a_HDC) : nullptr;
    WIN32_CHECK_ERROR(::ReleaseDC(hwnd, hdc));
}

void Win32::CheckError(const std::string& a_File, const uint32_t& a_Line, const std::string& a_Procedure)
{
    const auto errorCode = HRESULT_CODE(::GetLastError());
    if (errorCode == ERROR_SUCCESS)
        return;
    ::SetLastError(ERROR_SUCCESS);
    std::string errorString = a_File + "@" + std::to_string(a_Line) + " : " + a_Procedure + " failed, **ERROR CODE** : " + std::to_string(errorCode);
    std::cerr << errorString << std::endl;
    throw std::runtime_error(errorString);
}

int32_t Win32::GetDefaultPixelFormat(std::any a_DC)
{
    PIXELFORMATDESCRIPTOR pfd;
    std::memset(&pfd, 0, sizeof(pfd));
    pfd.nSize          = sizeof(pfd);
    pfd.nVersion       = 1;
    pfd.dwFlags        = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType     = PFD_TYPE_RGBA;
    pfd.iLayerType     = PFD_MAIN_PLANE;
    pfd.cColorBits     = 32;
    const auto pformat = ::ChoosePixelFormat(std::any_cast<HDC>(a_DC), &pfd);
    WIN32_CHECK_ERROR(pformat != 0);
    return pformat;
}

void Win32::SetPixelFormat(std::any a_HDC, const int32_t& a_PixelFormat)
{
    WIN32_CHECK_ERROR(::SetPixelFormat(std::any_cast<HDC>(a_HDC), a_PixelFormat, nullptr));
}
