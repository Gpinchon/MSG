#include <MSG/Renderer/OGL/Context.hpp>
#include <MSG/Renderer/OGL/Win32/PlatformCtx.hpp>
#include <MSG/Renderer/OGL/Win32/Win32.hpp>

void Platform::CtxDeleter::operator()(Platform::Ctx* a_Context) { delete a_Context; }

Platform::CtxHeadless::CtxHeadless(const MSG::Renderer::CreateContextInfo& a_Info)
    : Ctx(Win32::HDCWrapper::Create(Win32::HWNDWrapper::Create(Win32::WNDClassWrapper::Create("MSG::RendererContext"), "")), a_Info.setPixelFormat)
{
}

Platform::CtxNormal::CtxNormal(const MSG::Renderer::CreateContextInfo& a_Info)
    : Ctx(Win32::HDCWrapper::Create(a_Info.nativeDisplayHandle), a_Info.setPixelFormat)
{
}

uint64_t Platform::CtxGetID(const Platform::Ctx& a_Ctx)
{
    return WGL::GetID(a_Ctx.hglrc);
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
