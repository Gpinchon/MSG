#include <Renderer/SwapChain.hpp>

#include <Renderer/OCRA/Renderer.hpp>
#include <Renderer/OCRA/RenderBuffer.hpp>

#include <OCRA/SwapChain.hpp>

#ifdef WIN32
#include <Windows.h>
#endif

namespace TabGraph::Renderer::SwapChain {
auto CreateSurface(OCRA::Instance::Handle& a_Instance, void* a_HWND) {
    OCRA::CreateSurfaceInfo surfaceInfo{};
#ifdef WIN32
    surfaceInfo.hinstance = GetModuleHandle(0);
    surfaceInfo.hwnd = a_HWND;
#endif
    return OCRA::Instance::CreateSurface(a_Instance, surfaceInfo);
}
struct Impl {
    Impl(
        const Renderer::Handle& a_Renderer,
        const CreateSwapChainInfo& a_Info,
        const OCRA::Surface::Handle&   a_Surface,
        const OCRA::SwapChain::Handle& a_OldSwapChain = nullptr)
        : renderer(a_Renderer)
        , surface(CreateSurface(a_Renderer->instance, a_Info.hwnd))
    {
        auto formats = OCRA::PhysicalDevice::GetSurfaceFormats(a_Renderer->physicalDevice, surface);
        OCRA::CreateSwapChainInfo swapChainInfo;
        swapChainInfo.presentMode = a_Info.vSync ? OCRA::SwapChainPresentMode::Fifo : OCRA::SwapChainPresentMode::Immediate;
        swapChainInfo.surface = surface;
        swapChainInfo.imageUsage = OCRA::ImageUsageFlagBits::TransferDst;
        swapChainInfo.imageExtent.width = a_Info.width;
        swapChainInfo.imageExtent.height = a_Info.height;
        swapChainInfo.imageFormat = formats.front().format;
        swapChainInfo.imageCount = a_Info.imageCount;
        swapChainInfo.imageColorSpace = formats.front().colorSpace;
        swapChainInfo.imageArrayLayers = 1;
        swapChainInfo.oldSwapchain = a_OldSwapChain;
        swapChain = OCRA::Device::CreateSwapChain(a_Renderer->logicalDevice, swapChainInfo);
        auto imageCount = OCRA::SwapChain::GetImageCount(swapChain);
        for (auto i = 0; i < imageCount; ++i) {
            auto newFence = OCRA::Device::CreateFence(renderer.lock()->logicalDevice);
            imageAcquisitionFences.push_back(newFence);
        }
    }
    Impl(const Renderer::Handle& a_Renderer, const CreateSwapChainInfo& a_Info)
        : Impl(a_Renderer, a_Info, CreateSurface(a_Renderer->instance, a_Info.hwnd))
    {}
    Impl(const SwapChain::Handle& a_OldSwapChain, const CreateSwapChainInfo& a_Info)
        : Impl(renderer.lock(), a_Info, a_OldSwapChain->surface, a_OldSwapChain->swapChain)
    {}
    auto Present(const RenderBuffer::Handle& a_RenderBuffer) {
        auto timeout = std::chrono::nanoseconds(150000);
        auto& imageAcquisitionFence = imageAcquisitionFences.at(nextImageIndex);
        auto image = OCRA::SwapChain::GetNextImage(swapChain, timeout, nullptr, imageAcquisitionFence);
        nextImageIndex = image.second;
        OCRA::SwapChainPresentInfo presentInfo;
        presentInfo.swapChains = { swapChain };
        OCRA::Queue::Present(renderer.lock()->queue, presentInfo);
    }
    std::weak_ptr<Renderer::Impl> renderer;
    OCRA::Surface::Handle         surface;
    OCRA::SwapChain::Handle       swapChain;
    OCRA::Command::Buffer::Handle commandBuffer;
    uint8_t nextImageIndex = 0;
    std::vector<OCRA::Fence::Handle> imageAcquisitionFences;
};

Handle Create(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info)
{
    return Handle(new Impl(a_Renderer, a_Info));
}

Handle Recreate(
    const SwapChain::Handle& a_OldSwapChain,
    const CreateSwapChainInfo& a_Info)
{
    return Handle(new Impl(a_OldSwapChain, a_Info));
}

void Present(
    const SwapChain::Handle& a_SwapChain,
    const RenderBuffer::Handle& a_RenderBuffer)
{
    a_SwapChain->Present(a_RenderBuffer);
}

}