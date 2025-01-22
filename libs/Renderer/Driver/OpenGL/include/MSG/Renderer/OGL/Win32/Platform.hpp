#pragma once

#include <any>
#include <string>

namespace Platform {
void CheckError(const std::string& a_File, const uint32_t& a_Line, const std::string& a_Procedure);
void RegisterWindowClass(const std::string& a_ClassName);
void UnregisterWindowClass(const std::string& a_ClassName);
std::any CreateHWND(const std::string& a_ClassName, const std::string& a_Name);
void DestroyHWND(const std::any a_HWND);
std::any GetDC(std::any a_HWND);
void ReleaseDC(std::any a_HWND, std::any a_DC);

int32_t GetDefaultPixelFormat(std::any a_DC);
void SetPixelFormat(std::any a_HDC, const int32_t& a_PixelFormat);
}

#ifndef NDEBUG
// Similar to assert except it attempts to GetLastError to find more intel
#define WIN32_CHECK_ERROR(condition)                              \
    {                                                             \
        if (!(condition)) {                                       \
            Platform::CheckError(__FILE__, __LINE__, #condition); \
        };                                                        \
    }
#else
#define WIN32_CHECK_ERROR(condition) \
    {                                \
        if (!(condition)) { };       \
    }
#endif // NDEBUG
