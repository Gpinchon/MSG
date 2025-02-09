#include <MSG/OGLContext.hpp>
#include <MSG/OGLContext/PlatformCtx.hpp>
#include <MSG/OGLContext/X11.hpp>

void Platform::CtxDeleter::operator()(Platform::Ctx* a_Context) { delete a_Context; }

Platform::CtxHeadless::CtxHeadless(const MSG::OGLContextCreateInfo& a_Info)
    : Ctx(X11::OpenDisplay(),
          {},
          a_Info.sharedContext ? a_Info.sharedContext->impl.get() : nullptr,
          a_Info.setPixelFormat)
{
}

Platform::CtxHeadless::~CtxHeadless()
{
    X11::CloseDisplay(handleDisplay);
}

Platform::CtxNormal::CtxNormal(const MSG::OGLContextCreateInfo& a_Info)
    : Ctx(a_Info.nativeDisplayHandle,
          a_Info.nativeWindowHandle,
          a_Info.sharedContext ? a_Info.sharedContext->impl.get() : nullptr,
          a_Info.setPixelFormat)
{
}

uint64_t Platform::CtxGetID(const Platform::Ctx& a_Ctx)
{
    return GLX::GetID(a_Ctx.handle);
}

void Platform::CtxMakeCurrent(const Platform::Ctx& a_Ctx)
{
    if (a_Ctx.handleDrawable.has_value()) {
        return GLX::MakeCurrent(a_Ctx.handleDisplay, a_Ctx.handleDrawable, a_Ctx.handle);
    } else
        return GLX::MakeCurrent(a_Ctx.handleDisplay, a_Ctx.handle);
}

void Platform::CtxSwapBuffers(const Platform::Ctx& a_Ctx)
{
    GLX::SwapBuffers(a_Ctx.handleDisplay);
}

void Platform::CtxRelease(const Platform::Ctx& a_Ctx)
{
    GLX::Release(a_Ctx.handleDisplay);
}

void Platform::CtxSetSwapInterval(const Platform::Ctx& a_Ctx, const int8_t& a_Interval)
{
    GLX::SwapInterval(a_Ctx.handleDisplay, a_Interval);
}

template <typename ContextType>
Platform::Ctx* CreateContext(const MSG::OGLContextCreateInfo& a_Info)
{
    Platform::Ctx* ctx;
    if (a_Info.sharedContext != nullptr) {
        // if we want a shared context, we need to create it inside the shared context thread
        MSG::ExecuteOGLCommand(*a_Info.sharedContext, [&ctx, a_Info]() mutable { ctx = new ContextType(a_Info); }, true);
    } else
        ctx = new ContextType(a_Info);
    return ctx;
}

MSG::OGLContext MSG::CreateHeadlessOGLContext(const OGLContextCreateInfo& a_Info)
{
    return { a_Info, CreateContext<Platform::CtxHeadless>(a_Info) };
}

MSG::OGLContext MSG::CreateNormalOGLContext(const OGLContextCreateInfo& a_Info)
{
    return { a_Info, CreateContext<Platform::CtxNormal>(a_Info) };
}
