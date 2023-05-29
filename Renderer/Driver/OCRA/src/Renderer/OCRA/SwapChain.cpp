#include <Renderer/SwapChain.hpp>

#include <Renderer/OCRA/RenderBuffer.hpp>
#include <Renderer/OCRA/Renderer.hpp>

#include <OCRA/SwapChain.hpp>

namespace TabGraph::Renderer::SwapChain {
OCRA::Surface::Handle CreateSurface(const Renderer::Handle& a_Renderer, void* a_HWND);

auto CreateCommandPool(const Renderer::Handle& a_Renderer)
{
    OCRA::CreateCommandPoolInfo info;
    info.flags            = OCRA::CreateCommandPoolFlagBits::Reset;
    info.queueFamilyIndex = OCRA::PhysicalDevice::FindQueueFamily(a_Renderer->physicalDevice, OCRA::QueueFlagBits::Graphics);
    return OCRA::Device::CreateCommandPool(a_Renderer->logicalDevice, info);
}

auto CreateCommandBuffer(const Renderer::Handle& a_Renderer, const OCRA::Command::Pool::Handle& a_Pool)
{
    OCRA::AllocateCommandBufferInfo info;
    info.count = 1;
    info.level = OCRA::CommandBufferLevel::Primary;
    return OCRA::Command::Pool::AllocateCommandBuffer(a_Pool, info).front();
}

struct Impl {
    Impl(
        const Renderer::Handle& a_Renderer,
        const CreateSwapChainInfo& a_Info,
        const OCRA::Surface::Handle& a_Surface,
        const OCRA::SwapChain::Handle& a_OldSwapChain = nullptr)
        : renderer(a_Renderer)
        , surface(a_Surface)
    {
        auto formats = OCRA::PhysicalDevice::GetSurfaceFormats(a_Renderer->physicalDevice, surface);
        OCRA::CreateSwapChainInfo swapChainInfo;
        swapChainInfo.presentMode        = a_Info.vSync ? OCRA::SwapChainPresentMode::Fifo : OCRA::SwapChainPresentMode::Immediate;
        swapChainInfo.surface            = surface;
        swapChainInfo.imageUsage         = OCRA::ImageUsageFlagBits::TransferDst;
        swapChainInfo.imageExtent.width  = a_Info.width;
        swapChainInfo.imageExtent.height = a_Info.height;
        swapChainInfo.imageFormat        = formats.front().format;
        swapChainInfo.imageCount         = a_Info.imageCount;
        swapChainInfo.imageColorSpace    = formats.front().colorSpace;
        swapChainInfo.imageArrayLayers   = 1;
        swapChainInfo.oldSwapchain       = a_OldSwapChain;
        swapChain                        = OCRA::Device::CreateSwapChain(a_Renderer->logicalDevice, swapChainInfo);
        imageCount                       = OCRA::SwapChain::GetImageCount(swapChain);
    }
    Impl(const Renderer::Handle& a_Renderer, const CreateSwapChainInfo& a_Info)
        : Impl(a_Renderer, a_Info, CreateSurface(a_Renderer, a_Info.hwnd))
    {
        auto rendererPtr = renderer.lock();
        commandBuffer    = CreateCommandBuffer(a_Renderer, a_Renderer->commandPool);
        for (auto i = 0; i < imageCount; ++i) {
            auto newFence     = OCRA::Device::CreateFence(rendererPtr->logicalDevice);
            auto newFenceCopy = OCRA::Device::CreateFence(rendererPtr->logicalDevice);
            auto newSemaphore = OCRA::Device::CreateSemaphore(rendererPtr->logicalDevice, { OCRA::SemaphoreType::Binary });
            imageAcquisitionFences.push_back(newFence);
            imageCopySemaphores.push_back(newSemaphore);
            imageCopyFences.push_back(newFenceCopy);
        }
    }
    Impl(const SwapChain::Handle& a_OldSwapChain, const CreateSwapChainInfo& a_Info)
        : Impl(a_OldSwapChain->renderer.lock(), a_Info, a_OldSwapChain->surface, a_OldSwapChain->swapChain)
    {
        commandBuffer          = a_OldSwapChain->commandBuffer;
        imageAcquisitionFences = a_OldSwapChain->imageAcquisitionFences;
        imageCopySemaphores    = a_OldSwapChain->imageCopySemaphores;
        imageCopyFences        = a_OldSwapChain->imageCopyFences;
    }
    ~Impl()
    {
        OCRA::Queue::WaitIdle(renderer.lock()->queue);
        OCRA::Command::Buffer::Reset(commandBuffer);
    }
    auto Present(const RenderBuffer::Handle& a_RenderBuffer)
    {
        nextImageIndex              = (nextImageIndex + 1) % imageCount;
        auto rendererPtr            = renderer.lock();
        auto timeout                = std::chrono::nanoseconds(150000);
        auto& imageCopyFence        = imageCopyFences.at(nextImageIndex);
        auto& imageCopySemaphore    = imageCopySemaphores.at(nextImageIndex);
        auto& imageAcquisitionFence = imageAcquisitionFences.at(nextImageIndex);

        auto image = OCRA::SwapChain::GetNextImage(swapChain, timeout, nullptr, imageAcquisitionFence);
        OCRA::Fence::WaitFor(imageAcquisitionFence, OCRA::IgnoreTimeout);
        OCRA::Fence::Reset(imageAcquisitionFence);

        OCRA::CommandBufferBeginInfo beginInfo;
        beginInfo.flags = OCRA::CommandBufferUsageFlagBits::OneTimeSubmit;
        OCRA::Command::Buffer::Begin(commandBuffer, beginInfo);
        {
            auto& srcImage = a_RenderBuffer->image;
            auto& dstImage = image.first;
            {
                OCRA::ImageLayoutTransitionInfo srcTransition;
                srcTransition.oldLayout        = OCRA::ImageLayout::Undefined; // TODO use General
                srcTransition.newLayout        = OCRA::ImageLayout::TransferSrcOptimal;
                srcTransition.image            = srcImage;
                srcTransition.subRange.aspects = OCRA::ImageAspectFlagBits::Color;
                OCRA::ImageLayoutTransitionInfo dstTransition;
                dstTransition.oldLayout        = OCRA::ImageLayout::Undefined;
                dstTransition.newLayout        = OCRA::ImageLayout::TransferDstOptimal;
                dstTransition.image            = dstImage;
                dstTransition.subRange.aspects = OCRA::ImageAspectFlagBits::Color;
                OCRA::Command::TransitionImagesLayout(
                    commandBuffer, { srcTransition, dstTransition });
            }
            {
                OCRA::ImageBlit blit;
                blit.srcSubresource.aspects    = OCRA::ImageAspectFlagBits::Color;
                blit.srcOffsets.at(0).z        = 0;
                blit.srcOffsets.at(1).z        = 1;
                blit.srcSubresource.layerCount = 1;

                blit.dstSubresource.aspects    = OCRA::ImageAspectFlagBits::Color;
                blit.dstOffsets.at(0).z        = 0;
                blit.dstOffsets.at(1).z        = 1;
                blit.dstSubresource.layerCount = 1;

                OCRA::Command::BlitImage(
                    commandBuffer,
                    srcImage, dstImage,
                    { blit }, OCRA::Filter::Linear);
            }
            {
                OCRA::ImageLayoutTransitionInfo transition;
                transition.oldLayout        = OCRA::ImageLayout::TransferDstOptimal;
                transition.newLayout        = OCRA::ImageLayout::PresentSrc;
                transition.image            = dstImage;
                transition.subRange.aspects = OCRA::ImageAspectFlagBits::Color;
                OCRA::Command::TransitionImageLayout(
                    commandBuffer, transition);
            }
        }
        OCRA::Command::Buffer::End(commandBuffer);
        OCRA::QueueSubmitInfo submitInfo;
        submitInfo.commandBuffers   = { commandBuffer };
        submitInfo.signalSemaphores = { { imageCopySemaphore } };
        OCRA::Queue::Submit(rendererPtr->queue, { submitInfo }, imageCopyFence);
        OCRA::Fence::WaitFor(imageCopyFence, OCRA::IgnoreTimeout);
        OCRA::Fence::Reset(imageCopyFence);

        OCRA::SwapChainPresentInfo presentInfo;
        presentInfo.waitSemaphores = { imageCopySemaphore };
        presentInfo.swapChains     = { swapChain };
        OCRA::Queue::Present(rendererPtr->queue, presentInfo);
    }
    std::weak_ptr<Renderer::Impl> renderer;
    OCRA::Surface::Handle surface;
    OCRA::SwapChain::Handle swapChain;
    uint8_t imageCount;
    uint8_t nextImageIndex = 0;
    OCRA::Command::Buffer::Handle commandBuffer;
    std::vector<OCRA::Fence::Handle> imageCopyFences;
    std::vector<OCRA::Semaphore::Handle> imageCopySemaphores;
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

#ifdef WIN32
#include <Windows.h>
#endif
OCRA::Surface::Handle CreateSurface(const Renderer::Handle& a_Renderer, void* a_HWND)
{
    OCRA::CreateSurfaceInfo surfaceInfo {};
#ifdef WIN32
    surfaceInfo.hinstance = GetModuleHandle(0);
    surfaceInfo.hwnd      = a_HWND;
#endif
    return OCRA::Instance::CreateSurface(a_Renderer->instance, surfaceInfo);
}

}
