#include <MSG/OGLContext.hpp>
#include <MSG/OGLContext/Win32/PlatformCtx.hpp>
#include <MSG/OGLContext/Win32/Win32.hpp>

void Platform::CtxDeleter::operator()(Platform::Ctx* a_Context) { delete a_Context; }

static auto GetHeadlessHWND()
{
    return Win32::HDCWrapper::Create(Win32::HWNDWrapper::Create(Win32::WNDClassWrapper::Create(Win32::DefaultWindowClassName), ""));
}

static std::any GetSharedHGLRC(const Msg::OGLContext* a_Ctx)
{
    return a_Ctx ? a_Ctx->impl->hglrc : std::any {};
}

Platform::CtxHeadless::CtxHeadless(const Msg::OGLContextCreateInfo& a_Info)
    : Ctx(GetHeadlessHWND(), GetSharedHGLRC(a_Info.sharedContext), a_Info.setPixelFormat)
{
}

Platform::CtxNormal::CtxNormal(const Msg::OGLContextCreateInfo& a_Info)
    : Ctx(Win32::HDCWrapper::Create(a_Info.nativeDisplayHandle), GetSharedHGLRC(a_Info.sharedContext), a_Info.setPixelFormat)
{
}

uint64_t Platform::CtxGetID(const Platform::Ctx& a_Ctx)
{
    return WGL::GetID(a_Ctx.hglrc);
}

std::any Platform::CtxGetNativeHandle(const Platform::Ctx& a_Ctx)
{
    return a_Ctx.hglrc;
}

std::any Platform::CtxGetNativeSurface(const Ctx& a_Ctx)
{
    return a_Ctx.hdcWrapper->hdc;
}

void Platform::CtxMakeCurrent(const Platform::Ctx& a_Ctx)
{
    auto hdc = a_Ctx.hdcWrapper != nullptr ? a_Ctx.hdcWrapper->hdc : std::any {};
    return WGL::MakeCurrent(hdc, a_Ctx.hglrc);
}

void Platform::CtxSwapBuffers(const Platform::Ctx& a_Ctx)
{
    auto hdc = a_Ctx.hdcWrapper != nullptr ? a_Ctx.hdcWrapper->hdc : std::any {};
    return WGL::SwapBuffers(hdc);
}

void Platform::CtxRelease(const Platform::Ctx& a_Ctx)
{
    auto hdc = a_Ctx.hdcWrapper != nullptr ? a_Ctx.hdcWrapper->hdc : std::any {};
    WGL::Release(hdc);
}

void Platform::CtxSetSwapInterval(const Ctx& a_Ctx, const int8_t& a_Interval)
{
    auto hdc = a_Ctx.hdcWrapper != nullptr ? a_Ctx.hdcWrapper->hdc : std::any {};
    return WGL::SwapInterval(hdc, a_Interval);
}

template <typename ContextType>
Platform::Ctx* CreateContext(const Msg::OGLContextCreateInfo& a_Info)
{
    Platform::Ctx* ctx;
    if (a_Info.sharedContext != nullptr) {
        // if we want a shared context, we need to create it inside the shared context thread
        Msg::ExecuteOGLCommand(*a_Info.sharedContext, [&ctx, a_Info]() mutable { ctx = new ContextType(a_Info); }, true);
    } else
        ctx = new ContextType(a_Info);
    return ctx;
}

Msg::OGLContext Msg::CreateHeadlessOGLContext(const OGLContextCreateInfo& a_Info)
{
    return { a_Info, CreateContext<Platform::CtxHeadless>(a_Info) };
}

Msg::OGLContext Msg::CreateNormalOGLContext(const OGLContextCreateInfo& a_Info)
{
    return { a_Info, CreateContext<Platform::CtxNormal>(a_Info) };
}
