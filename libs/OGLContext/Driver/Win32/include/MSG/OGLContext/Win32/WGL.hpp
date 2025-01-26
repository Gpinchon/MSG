#pragma once

#include <any>
#include <memory>

namespace Win32 {
class HDCWrapper;
}

namespace WGL {
class HGLRCWrapper {
public:
    HGLRCWrapper(const std::shared_ptr<Win32::HDCWrapper>& a_HDCWrapper, const HGLRCWrapper* a_SharedHGLRC, const bool& a_SetPixelFormat);
    ~HGLRCWrapper();
    std::shared_ptr<Win32::HDCWrapper> hdcWrapper;
    std::any hglrc;
};

void InitOGL();
std::any CreateContext(const std::any& a_HDC, const HGLRCWrapper* a_SharedHGLRC);
int32_t GetDefaultPixelFormat(const std::any& a_HDC);
uint64_t GetID(const std::any& a_HGLRC);
void MakeCurrent(const std::any& a_HDC, const std::any& a_HGLRC);
void SwapBuffers(const std::any& a_HDC);
void Release(const std::any& a_HDC);
void SwapInterval(const std::any& a_HDC, const int8_t& a_Interval);
}
