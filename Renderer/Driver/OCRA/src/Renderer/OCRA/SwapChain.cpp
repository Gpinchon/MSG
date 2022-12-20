#include <Renderer/SwapChain.hpp>

#include <Renderer/OCRA/Renderer.hpp>
#include <Renderer/OCRA/RenderBuffer.hpp>

#include <OCRA/SwapChain.hpp>
#include <OCRA/Surface.hpp>

#ifdef WIN32
#include <Windows.h>
#endif

namespace TabGraph::Renderer::SwapChain {
struct Impl {
    Impl(const Renderer::Handle& a_Renderer, const Info& a_Info)
        : renderer(a_Renderer)
    {
#ifdef WIN32
        {
            OCRA::Surface::Win32::Info surfaceInfo{};
            surfaceInfo.hinstance = GetModuleHandle(0);
            surfaceInfo.hwnd = a_Info.hwnd;
            surface = OCRA::Surface::Win32::Create(a_Renderer->instance, surfaceInfo);
        }
#endif
        {
            OCRA::SwapChain::Info swapChainInfo;
            swapChainInfo.presentMode = a_Info.vSync ? OCRA::SwapChain::PresentMode::Fifo : OCRA::SwapChain::PresentMode::Immediate;
            swapChainInfo.surface = surface;
            swapChainInfo.imageExtent.width = a_Info.width;
            swapChainInfo.imageExtent.height = a_Info.height;
            swapChainInfo.imageFormat = OCRA::Image::Format::Uint8_Normalized_RGB;
            swapChainInfo.imageCount = a_Info.imageCount;
            swapChainInfo.imageColorSpace = OCRA::Image::ColorSpace::sRGB;
            swapChain = OCRA::SwapChain::Create(a_Renderer->logicalDevice, swapChainInfo);
            nextRenderBuffer = GetNextRenderBuffer();
        }
    }
    Impl(const SwapChain::Handle& a_OldSwapChain, const Info& a_Info)
        : renderer(a_OldSwapChain->renderer)
        , surface(a_OldSwapChain->surface)
    {
        OCRA::SwapChain::Info swapChainInfo;
        swapChainInfo.presentMode = a_Info.vSync ? OCRA::SwapChain::PresentMode::Fifo : OCRA::SwapChain::PresentMode::Immediate;
        swapChainInfo.surface = a_OldSwapChain->surface;
        swapChainInfo.imageExtent.width = a_Info.width;
        swapChainInfo.imageExtent.height = a_Info.height;
        swapChainInfo.imageFormat = OCRA::Image::Format::Uint8_Normalized_RGB;
        swapChainInfo.imageCount = a_Info.imageCount;
        swapChainInfo.imageColorSpace = OCRA::Image::ColorSpace::sRGB;
        swapChainInfo.oldSwapchain = a_OldSwapChain->swapChain;
        swapChain = OCRA::SwapChain::Create(renderer.lock()->logicalDevice, swapChainInfo);
        nextRenderBuffer = GetNextRenderBuffer();
    }
    auto Present() {
        OCRA::SwapChain::PresentInfo presentInfo;
        presentInfo.swapChains = { swapChain };
        OCRA::SwapChain::Present(renderer.lock()->queue, presentInfo);
        nextRenderBuffer = GetNextRenderBuffer();
    }
    auto GetNextRenderBufferStorage() {
        //we don't need to reallocate this
        auto ptr = nextRenderBufferStorage + sizeof(RenderBuffer::Impl) * nextRenderBufferOffset;
        ++nextRenderBufferOffset;
        nextRenderBufferOffset = nextRenderBufferOffset % 2;
        return ptr;
    }
    RenderBuffer::Handle GetNextRenderBuffer() {
        auto timeout = std::chrono::nanoseconds(150000);
        auto image = OCRA::SwapChain::AcquireNextImage(renderer.lock()->logicalDevice, swapChain, timeout, nullptr, nullptr);
        auto renderBuffer = new(GetNextRenderBufferStorage()) RenderBuffer::Impl(image);
        return RenderBuffer::Handle(renderBuffer, [](RenderBuffer::Impl* a_Ptr) {
            std::destroy_at(a_Ptr);
        });
    }
    std::weak_ptr<Renderer::Impl> renderer;
    OCRA::Surface::Handle surface;
    OCRA::SwapChain::Handle swapChain;
    RenderBuffer::Handle nextRenderBuffer;
    uint8_t nextRenderBufferOffset{ 0 };
    alignas(RenderBuffer::Impl) std::byte nextRenderBufferStorage[sizeof(RenderBuffer::Impl) * 2];
};

Handle Create(
    const Renderer::Handle& a_Renderer,
    const Info& a_Info)
{
    return Handle(new Impl(a_Renderer, a_Info));
}

Handle Recreate(
    const SwapChain::Handle& a_OldSwapChain,
    const Info& a_Info)
{
    return Handle(new Impl(a_OldSwapChain, a_Info));
}

void Present(const SwapChain::Handle& a_SwapChain)
{
    a_SwapChain->Present();
}

RenderBuffer::Handle GetNextBuffer(const Handle& a_SwapChain)
{
    return a_SwapChain->nextRenderBuffer;
}

}