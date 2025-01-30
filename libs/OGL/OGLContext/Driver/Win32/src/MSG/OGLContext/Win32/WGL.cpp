#include <MSG/OGLContext.hpp>
#include <MSG/OGLContext/Win32/WGL.hpp>
#include <MSG/OGLContext/Win32/Win32.hpp>

#include <stdexcept>

#define NOMSG
#include <GL/glew.h>
#include <GL/wglew.h>

constexpr int PfdAttribs[] = {
    WGL_SUPPORT_OPENGL_ARB, true,
    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
    WGL_COLORSPACE_EXT, WGL_COLORSPACE_SRGB_EXT,
    WGL_COLOR_BITS_ARB, 32,
    WGL_DEPTH_BITS_ARB, 0,
    WGL_STENCIL_BITS_ARB, 0,
    WGL_ACCUM_BITS_ARB, 0,
    WGL_AUX_BUFFERS_ARB, 0,
    0
};

constexpr int CtxAttribs[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
    WGL_CONTEXT_MINOR_VERSION_ARB, 3,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifndef NDEBUG
    WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
#else
    WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB,
#endif // NDEBUG
    0
};

void WGL::InitOGL()
{
    static bool s_Initialized = false;
    if (s_Initialized)
        return; // we only need to initialize OGL once for the whole
    const auto tempHWNDWrapper = Win32::HWNDWrapper::Create(Win32::WNDClassWrapper::Create("OpenGL::Initialize"), "OpenGL::Initialize");
    const auto tempHDCWrapper  = Win32::HDCWrapper::Create(tempHWNDWrapper);
    const auto tempDC          = std::any_cast<HDC>(tempHDCWrapper->hdc);
    Win32::SetPixelFormat(tempDC, Win32::GetDefaultPixelFormat(tempDC));
    const auto tempHGLRC = wglCreateContext(tempDC);
    glewExperimental     = true;
    WIN32_CHECK_ERROR(tempHGLRC != nullptr);
    WIN32_CHECK_ERROR(wglMakeCurrent(tempDC, tempHGLRC));
    WIN32_CHECK_ERROR(glewInit() == GLEW_OK); // load OGL extensions
    WIN32_CHECK_ERROR(wglewInit() == GLEW_OK); // load WGL extensions
    WIN32_CHECK_ERROR(wglMakeCurrent(tempDC, nullptr));
    WIN32_CHECK_ERROR(wglDeleteContext(tempHGLRC));
    s_Initialized = true; // OGL was initialized, no need to do it again next time
}

std::any WGL::CreateContext(const std::any& a_HDC, const HGLRCWrapper* a_SharedHGLRC)
{
    if (!WGLEW_ARB_create_context)
        throw std::runtime_error("Modern context creation not supported !");
    if (!WGLEW_ARB_create_context_robustness)
        throw std::runtime_error("Robust context creation not supported !");
    HDC hdc           = std::any_cast<HDC>(a_HDC);
    HGLRC hglrcShared = a_SharedHGLRC != nullptr ? std::any_cast<HGLRC>(a_SharedHGLRC->hglrc) : nullptr;
    auto hglrc        = wglCreateContextAttribsARB(hdc, hglrcShared, CtxAttribs);
    WIN32_CHECK_ERROR(hglrc != nullptr);
    return hglrc;
}

int32_t WGL::GetDefaultPixelFormat(const std::any& a_HDC)
{
    auto hdc                = std::any_cast<HDC>(a_HDC);
    int32_t pixelFormat     = 0;
    uint32_t pixelFormatNbr = 0;
    WIN32_CHECK_ERROR(wglChoosePixelFormatARB(hdc, PfdAttribs, nullptr, 1, &pixelFormat, &pixelFormatNbr));
    WIN32_CHECK_ERROR(pixelFormat != 0);
    WIN32_CHECK_ERROR(pixelFormatNbr != 0);
    return pixelFormat;
}

WGL::HGLRCWrapper::HGLRCWrapper(const std::shared_ptr<Win32::HDCWrapper>& a_HDCWrapper, const HGLRCWrapper* a_SharedHGLRC, const bool& a_SetPixelFormat)
    : hdcWrapper(a_HDCWrapper)
{
    WGL::InitOGL();
    if (a_SetPixelFormat)
        Win32::SetPixelFormat(hdcWrapper->hdc, WGL::GetDefaultPixelFormat(hdcWrapper->hdc));
    hglrc = CreateContext(hdcWrapper->hdc, a_SharedHGLRC);
}

WGL::HGLRCWrapper::~HGLRCWrapper()
{
    if (!hglrc.has_value())
        return;
    WIN32_CHECK_ERROR(wglDeleteContext(std::any_cast<HGLRC>(hglrc)));
}

uint64_t WGL::GetID(const std::any& a_HGLRC)
{
    return uint64_t(std::any_cast<HGLRC>(a_HGLRC));
}

void WGL::MakeCurrent(const std::any& a_HDC, const std::any& a_HGLRC)
{
    auto hdc   = a_HDC.has_value() ? std::any_cast<HDC>(a_HDC) : nullptr;
    auto hglrc = a_HGLRC.has_value() ? std::any_cast<HGLRC>(a_HGLRC) : nullptr;
    WIN32_CHECK_ERROR(wglMakeCurrent(hdc, hglrc));
}

void WGL::SwapBuffers(const std::any& a_HDC)
{
    auto hdc = a_HDC.has_value() ? std::any_cast<HDC>(a_HDC) : nullptr;
    WIN32_CHECK_ERROR(wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE));
}

void WGL::Release(const std::any& a_HDC)
{
    auto hdc = a_HDC.has_value() ? std::any_cast<HDC>(a_HDC) : nullptr;
    WIN32_CHECK_ERROR(wglMakeCurrent(hdc, nullptr));
}

void WGL::SwapInterval(const std::any& a_HDC, const int8_t& a_Interval)
{
    (void)a_HDC;
    WIN32_CHECK_ERROR(wglSwapIntervalEXT(a_Interval));
}
