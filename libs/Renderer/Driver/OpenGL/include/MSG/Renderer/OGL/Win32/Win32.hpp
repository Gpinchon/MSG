#pragma once

#include <any>
#include <memory>
#include <string>

namespace Win32 {
void CheckError(const std::string& a_File, const uint32_t& a_Line, const std::string& a_Procedure);
void RegisterWindowClass(const std::string& a_ClassName);
void UnregisterWindowClass(const std::string& a_ClassName);
std::any GetDesktopWindow();
std::any CreateHWND(const std::string& a_ClassName, const std::string& a_Name);
void DestroyHWND(const std::any a_HWND);
std::any GetDC(std::any a_HWND);
std::any GetCompatibleDC(std::any a_HDC = {});
std::any GetScreenDC();
void DeleteDC(std::any a_HDC);
void ReleaseDC(std::any a_HWND, std::any a_DC);

int32_t GetDefaultPixelFormat(std::any a_DC);
void SetPixelFormat(std::any a_HDC, const int32_t& a_PixelFormat);

class WNDClassWrapper {
public:
    WNDClassWrapper(std::string_view a_ClassName)
        : name(a_ClassName)
    {
        Win32::RegisterWindowClass(name);
    }
    WNDClassWrapper(const WNDClassWrapper&) = delete;
    ~WNDClassWrapper() { Win32::UnregisterWindowClass(name); }
    static std::shared_ptr<WNDClassWrapper> Create(std::string_view a_ClassName);
    const std::string name;
};

class HWNDWrapper {
public:
    HWNDWrapper(const std::shared_ptr<WNDClassWrapper>& a_Class, std::string_view a_WndName)
        : wndClassWrapper(a_Class)
        , hwnd(Win32::CreateHWND(a_Class->name, std::string(a_WndName)))
    {
    }
    HWNDWrapper(const HWNDWrapper&) = delete;
    ~HWNDWrapper() { Win32::DestroyHWND(hwnd); }
    static auto Create(const std::shared_ptr<WNDClassWrapper>& a_ClassWrapper, std::string_view a_WndName) { return std::make_shared<HWNDWrapper>(a_ClassWrapper, a_WndName); }
    const std::shared_ptr<WNDClassWrapper> wndClassWrapper;
    const std::any hwnd;
};

class HDCWrapper {
public:
    HDCWrapper(const std::shared_ptr<HWNDWrapper>& a_HwndWrapper)
        : hwndWrapper(a_HwndWrapper)
        , hdc(Win32::GetDC(hwndWrapper->hwnd))
    {
    }
    HDCWrapper(const std::any& a_HDC)
        : hdc(a_HDC)
    {
    }
    HDCWrapper(const HDCWrapper&) = delete;
    ~HDCWrapper()
    {
        if (hwndWrapper != nullptr)
            Win32::ReleaseDC(hwndWrapper->hwnd, hdc);
    }
    static auto Create(const std::any& a_HDC) { return std::make_shared<HDCWrapper>(a_HDC); }
    static auto Create(const std::shared_ptr<HWNDWrapper>& a_WndWrapper) { return std::make_shared<HDCWrapper>(a_WndWrapper); }
    const std::shared_ptr<HWNDWrapper> hwndWrapper;
    const std::any hdc;
};
}

#ifndef NDEBUG
// Similar to assert except it attempts to GetLastError to find more intel
#define WIN32_CHECK_ERROR(condition)                           \
    {                                                          \
        if (!(condition)) {                                    \
            Win32::CheckError(__FILE__, __LINE__, #condition); \
        };                                                     \
    }
#else
#define WIN32_CHECK_ERROR(condition) \
    {                                \
        if (!(condition)) { };       \
    }
#endif // NDEBUG
