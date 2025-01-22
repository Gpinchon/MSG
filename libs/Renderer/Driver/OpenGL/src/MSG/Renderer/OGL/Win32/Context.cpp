#include <MSG/Renderer/OGL/Context.hpp>
#include <MSG/Renderer/OGL/Win32/Context.hpp>
#include <MSG/Renderer/OGL/Win32/Platform.hpp>

#include <stdexcept>

#define NOMSG
#include <GL/glew.h>
#include <GL/wglew.h>

static void InitOGL()
{
    static bool s_Initialized = false;
    if (s_Initialized)
        return; // we only need to initialize OGL once for the whole
    const auto tempHWND = Platform::CreateHWND("OpenGL::Initialize", "OpenGL::Initialize");
    const auto tempDC   = std::any_cast<HDC>(Platform::GetDC(tempHWND));
    const auto tempPF   = Platform::GetDefaultPixelFormat(tempDC);
    Platform::SetPixelFormat(tempDC, tempPF);
    const auto tempHGLRC = wglCreateContext(std::any_cast<HDC>(tempDC));
    glewExperimental     = true;
    WIN32_CHECK_ERROR(tempHGLRC != nullptr);
    WIN32_CHECK_ERROR(wglMakeCurrent(tempDC, tempHGLRC));
    WIN32_CHECK_ERROR(glewInit() == GLEW_OK); // load OGL extensions
    WIN32_CHECK_ERROR(wglewInit() == GLEW_OK); // load WGL extensions
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(tempHGLRC);
    Platform::ReleaseDC(tempHWND, tempDC);
    Platform::DestroyHWND(tempHWND);
    s_Initialized = true; // OGL was initialized, no need to do it again next time
}

static HGLRC CreateContext(const std::any a_HDC, const MSG::Renderer::Context* a_SharedCtx)
{
    if (!WGLEW_ARB_create_context)
        throw std::runtime_error("Modern context creation not supported !");
    if (!WGLEW_ARB_create_context_robustness)
        throw std::runtime_error("Robust context creation not supported !");
    auto hdc            = std::any_cast<HDC>(a_HDC);
    auto hglrcShared    = a_SharedCtx != nullptr ? std::any_cast<HGLRC>(a_SharedCtx->impl->hglrc) : HGLRC(nullptr);
    const int attribs[] = {
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
    auto hglrc = wglCreateContextAttribsARB(hdc, hglrcShared, attribs);
    WIN32_CHECK_ERROR(hglrc != nullptr);
    return hglrc;
}

static void SetHeadlessPixelFormat(std::any a_HDC)
{
    auto hdc                    = std::any_cast<HDC>(a_HDC);
    constexpr int attribIList[] = {
        WGL_SUPPORT_OPENGL_ARB, true,
        WGL_COLOR_BITS_ARB, 0,
        WGL_ALPHA_BITS_ARB, 0,
        WGL_DEPTH_BITS_ARB, 0,
        WGL_STENCIL_BITS_ARB, 0,
        WGL_ACCUM_BITS_ARB, 0,
        WGL_AUX_BUFFERS_ARB, 0,
        0
    };
    int32_t pixelFormat     = 0;
    uint32_t pixelFormatNbr = 0;
    WIN32_CHECK_ERROR(wglChoosePixelFormatARB(hdc, attribIList, nullptr, 1, &pixelFormat, &pixelFormatNbr));
    WIN32_CHECK_ERROR(pixelFormat != 0);
    WIN32_CHECK_ERROR(pixelFormatNbr != 0);
    Platform::SetPixelFormat(hdc, pixelFormat);
}

static void SetNormalPixelFormat(std::any a_HDC, const MSG::Renderer::ContextPixelFormat& a_PixelFormat)
{
    const auto hdc          = std::any_cast<HDC>(a_HDC);
    const int attribIList[] = {
        WGL_DRAW_TO_WINDOW_ARB, true,
        WGL_SUPPORT_OPENGL_ARB, true,
        WGL_DOUBLE_BUFFER_ARB, true,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLORSPACE_EXT, (a_PixelFormat.sRGB ? WGL_COLORSPACE_SRGB_EXT : WGL_COLORSPACE_LINEAR_EXT),
        WGL_RED_BITS_ARB, a_PixelFormat.redBits,
        WGL_GREEN_BITS_ARB, a_PixelFormat.greenBits,
        WGL_BLUE_BITS_ARB, a_PixelFormat.blueBits,
        WGL_ALPHA_BITS_ARB, a_PixelFormat.alphaBits,
        WGL_DEPTH_BITS_ARB, a_PixelFormat.depthBits,
        WGL_STENCIL_BITS_ARB, a_PixelFormat.stencilBits,
        0
    };
    int32_t pixelFormat     = 0;
    uint32_t pixelFormatNbr = 0;
    WIN32_CHECK_ERROR(wglChoosePixelFormatARB(hdc, attribIList, nullptr, 1, &pixelFormat, &pixelFormatNbr));
    WIN32_CHECK_ERROR(pixelFormat != 0);
    WIN32_CHECK_ERROR(pixelFormatNbr != 0);
    Platform::SetPixelFormat(hdc, pixelFormat);
}

Platform::Context::~Context()
{
    if (!hglrc.has_value())
        return;
    wglDeleteContext(std::any_cast<HGLRC>(hglrc));
}

Platform::HeadlessContext::HeadlessContext(const MSG::Renderer::CreateContextInfo& a_Info)
    : hwnd(Platform::CreateHWND("Dummy", "Dummy"))
{
    hdc = Platform::GetDC(hwnd);
    InitOGL();
    SetHeadlessPixelFormat(hdc);
    hglrc = CreateContext(hdc, a_Info.sharedContext);
}

Platform::HeadlessContext ::~HeadlessContext()
{
    Platform::ReleaseDC(hwnd, hdc);
    Platform::DestroyHWND(hwnd);
}

Platform::NormalContext::NormalContext(const MSG::Renderer::CreateContextInfo& a_Info)
{
    hdc = a_Info.nativeDisplayHandle;
    if (a_Info.setPixelFormat)
        SetNormalPixelFormat(hdc, a_Info.pixelFormat);
    hglrc = CreateContext(hdc, a_Info.sharedContext);
}

uint64_t Platform::CtxGetID(const Platform::Context& a_Ctx)
{
    return uint64_t(std::any_cast<HGLRC>(a_Ctx.hglrc));
}

void Platform::CtxMakeCurrent(const Platform::Context& a_Ctx)
{
    WIN32_CHECK_ERROR(wglMakeCurrent(std::any_cast<HDC>(a_Ctx.hdc), std::any_cast<HGLRC>(a_Ctx.hglrc)));
}

void Platform::CtxSwapBuffers(const Platform::Context& a_Ctx)
{
    WIN32_CHECK_ERROR(wglSwapLayerBuffers(std::any_cast<HDC>(a_Ctx.hdc), WGL_SWAP_MAIN_PLANE));
}

void Platform::CtxRelease()
{
    WIN32_CHECK_ERROR(wglMakeCurrent(nullptr, nullptr));
}

void Platform::CtxSetSwapInterval(const int8_t& a_Interval)
{
    WIN32_CHECK_ERROR(wglSwapIntervalEXT(a_Interval));
}
