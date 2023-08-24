#include <Renderer/OGL/SwapChain.hpp>

#include <Renderer/RenderBuffer.hpp>

#include <cassert>

#ifdef WIN32
#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/OpenGL.hpp>
#include <Windows.h>
#endif

namespace TabGraph::Renderer::SwapChain {
Handle Create(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info)
{
    return std::make_shared<Impl>(a_Renderer, a_Info);
}

Handle Recreate(
    const SwapChain::Handle& a_OldSwapChain,
    const CreateSwapChainInfo& a_Info)
{
    return std::make_shared<Impl>(a_OldSwapChain, a_Info);
}

void Present(
    const SwapChain::Handle& a_SwapChain,
    const RenderBuffer::Handle& a_RenderBuffer)
{
}

Impl::Impl(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info)
    : renderer(a_Renderer)
    , width(a_Info.width)
    , height(a_Info.height)
    , imageCount(a_Info.imageCount)
    , vSync(a_Info.vSync)
    , hwnd(a_Info.hwnd)
    , hdc(GetDC(HWND(hwnd)))
{
    WIN32_CHECK_ERROR(hdc != nullptr);
    CreateRenderBufferInfo renderBufferInfo;
    renderBufferInfo.width  = width;
    renderBufferInfo.height = height;
    for (uint8_t index = 0; index < imageCount; ++index) {
        renderBuffers.at(index) = RenderBuffer::Create(a_Renderer, renderBufferInfo);
    }
#ifdef WIN32
    OpenGL::Win32::SetDefaultPixelFormat(hdc);
    hglrc = OpenGL::Win32::CreateContext(hdc);
    presentThread.PushCommand(
        [this] {
            WIN32_CHECK_ERROR(wglMakeCurrent(HDC(hdc), HGLRC(hglrc)));
        },
        true);
#endif
}

Impl::Impl(
    const Handle& a_OldSwapChain,
    const CreateSwapChainInfo& a_Info)
    : renderer(a_OldSwapChain->renderer)
    , width(a_Info.width)
    , height(a_Info.height)
    , imageCount(a_Info.imageCount)
    , vSync(a_Info.vSync)
    , hwnd(a_OldSwapChain->hwnd)
    , hdc(a_OldSwapChain->hdc)
    , hglrc(a_OldSwapChain->hglrc)
{
    uint8_t index = 0;
    if (a_OldSwapChain->width == a_Info.width && a_OldSwapChain->height == a_Info.height) {
        while (index < a_OldSwapChain->imageCount && index < imageCount) {
            renderBuffers.at(index) = a_OldSwapChain->renderBuffers.at(index);
            ++index;
        }
    }
    a_OldSwapChain->Retire();
    if (index < imageCount) {
        // Create the remaining render buffers
        CreateRenderBufferInfo renderBufferInfo;
        renderBufferInfo.width  = width;
        renderBufferInfo.height = height;
        while (index < imageCount) {
            renderBuffers.at(index) = RenderBuffer::Create(renderer.lock(), renderBufferInfo);
            ++index;
        }
    }
    presentThread.PushCommand([this] { WIN32_CHECK_ERROR(wglMakeCurrent(HDC(hdc), HGLRC(hglrc))); }, true);
}

Impl ::~Impl()
{
    retired = true;
#ifdef WIN32
    presentThread.PushCommand([this] { wglMakeCurrent(nullptr, nullptr); }, true);
    if (hglrc != nullptr)
        WIN32_CHECK_ERROR(wglDeleteContext(HGLRC(hglrc)));
    if (hdc != nullptr)
        ReleaseDC(HWND(hwnd), HDC(hdc));
#endif
}

void Impl::Retire()
{
#ifdef WIN32
    presentThread.PushCommand([this] { wglMakeCurrent(nullptr, nullptr); }, true);
    hdc   = nullptr;
    hglrc = nullptr;
#endif
    retired = true;
}
}
