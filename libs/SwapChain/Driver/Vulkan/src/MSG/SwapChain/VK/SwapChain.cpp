#include <MSG/PixelDescriptor.hpp>
#include <MSG/Renderer/RenderBuffer.hpp>
#include <MSG/Renderer/VK/Invoke.hpp>
#include <MSG/Renderer/VK/RenderBuffer.hpp>
#include <MSG/Renderer/VK/Renderer.hpp>
#include <MSG/SwapChain/SwapChain.hpp>
#include <MSG/SwapChain/VK/SwapChain.hpp>
#include <MSG/Debug.hpp>

namespace Msg::SwapChain {
// wait for a whole minute at most
constexpr uint64_t s_SwapChainTimeout = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::minutes(1)).count();

vk::PresentModeKHR GetVKPresentMode(const PresentMode& a_PresentMode)
{
    vk::PresentModeKHR interval = vk::PresentModeKHR::eImmediate;
    switch (a_PresentMode) {
    case PresentMode::Immediate:
        interval = vk::PresentModeKHR::eImmediate;
        break;
    case PresentMode::FIFO:
        interval = vk::PresentModeKHR::eFifo;
        break;
    case PresentMode::MailBox:
        interval = vk::PresentModeKHR::eMailbox;
        break;
    case PresentMode::FIFORelaxed:
        interval = vk::PresentModeKHR::eFifoRelaxed;
        break;
    default:
        break;
    }
    return interval;
}

vk::SurfaceFormatKHR GetSurfaceFormat(const vk::raii::PhysicalDevice& a_PhysicalDevice, const vk::raii::SurfaceKHR& a_Surface)
{
    for (auto& format : a_PhysicalDevice.getSurfaceFormatsKHR(*a_Surface)) {
        if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            return format;
    }
    MSGErrorFatal("Could not find proper surface format !");
}

static std::vector<vk::raii::CommandBuffer> CreateCommandBuffers(
    const vk::raii::Device& a_Device,
    const vk::raii::CommandPool& a_CommandPool,
    const uint32_t& a_Count               = 1,
    const vk::CommandBufferLevel& a_Level = vk::CommandBufferLevel::ePrimary)
{
    vk::CommandBufferAllocateInfo allocInfo { };
    allocInfo.commandPool        = *a_CommandPool;
    allocInfo.level              = a_Level;
    allocInfo.commandBufferCount = a_Count;
    return a_Device.allocateCommandBuffers(allocInfo);
}

static vk::SwapchainCreateInfoKHR GetVKCreateInfo(
    const CreateSwapChainInfo& a_Info,
    const vk::raii::PhysicalDevice& a_PhysicalDevice,
    const vk::raii::SurfaceKHR& a_Surface,
    const vk::SurfaceFormatKHR& a_SurfaceFormat,
    const Handle& a_OldSwapChain = nullptr)
{
    vk::SwapchainCreateInfoKHR info;
    auto surfaceFormats = GetSurfaceFormat(a_PhysicalDevice, a_Surface);
    info.setImageFormat(a_SurfaceFormat.format);
    info.setImageColorSpace(a_SurfaceFormat.colorSpace);
    info.setImageUsage(vk::ImageUsageFlagBits::eTransferDst);
    info.setImageArrayLayers(1);
    info.setImageExtent(vk::Extent2D(a_Info.width, a_Info.height));
    info.setMinImageCount(a_Info.imageCount);
    info.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
    info.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);
    info.setPresentMode(GetVKPresentMode(a_Info.presentMode));
    info.setSurface(*a_Surface);
    if (a_OldSwapChain != nullptr)
        info.setOldSwapchain(*a_OldSwapChain->swapChain);
    return info;
}

auto CreateSemaphores(vk::raii::Device& a_Device, const size_t& a_Count)
{
    std::vector<vk::raii::Semaphore> semaphores;
    semaphores.reserve(a_Count);
    for (size_t i = 0; i < a_Count; i++) {
        semaphores.emplace_back(a_Device, vk::SemaphoreCreateInfo { });
    }
    return std::move(semaphores);
}

auto CreateFences(vk::raii::Device& a_Device, const size_t& a_Count)
{
    std::vector<vk::raii::Fence> fences;
    fences.reserve(a_Count);
    for (size_t i = 0; i < a_Count; i++) {
        fences.emplace_back(a_Device, vk::FenceCreateInfo { vk::FenceCreateFlagBits::eSignaled });
    }
    return std::move(fences);
}

Impl::Impl(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info)
    : renderer(a_Renderer)
    , surface(renderer->instance, a_Info)
    , surfaceFormat(GetSurfaceFormat(renderer->physicalDevice, surface))
    , swapChain(renderer->device, GetVKCreateInfo(a_Info, renderer->physicalDevice, surface, surfaceFormat))
    , images(swapChain.getImages())
    , extent(a_Info.width, a_Info.height)
    , acqSemaphores(CreateSemaphores(renderer->device, MAX_FRAMES_IN_FLIGHT))
    , inFlightFences(CreateFences(renderer->device, MAX_FRAMES_IN_FLIGHT))
    , renderCompleteSemaphores(CreateSemaphores(renderer->device, images.size()))
    , presentCmdBuffers(std::move(CreateCommandBuffers(renderer->device, renderer->cmdPool, MAX_FRAMES_IN_FLIGHT)))
{
}

Impl::Impl(
    const Handle& a_OldSwapChain,
    const CreateSwapChainInfo& a_Info)
    : renderer(a_OldSwapChain->renderer)
    , surface(std::move(a_OldSwapChain->surface))
    , surfaceFormat(GetSurfaceFormat(renderer->physicalDevice, surface))
    , swapChain(renderer->device, GetVKCreateInfo(a_Info, renderer->physicalDevice, surface, surfaceFormat, a_OldSwapChain))
    , images(swapChain.getImages())
    , extent(a_Info.width, a_Info.height)
    , acqSemaphores(std::move(a_OldSwapChain->acqSemaphores))
    , inFlightFences(std::move(a_OldSwapChain->inFlightFences))
    , renderCompleteSemaphores(std::move(a_OldSwapChain->renderCompleteSemaphores))
    , presentCmdBuffers(std::move(a_OldSwapChain->presentCmdBuffers))
{
}

Impl::~Impl()
{
    Wait();
    renderer->device.waitIdle();
}

void Impl::AcquireNextImage()
{
    auto& inFlightFence = inFlightFences[frameIndex];
    // Wait for previous blit operation to be done
    VK_INVOKE(renderer->device.waitForFences(*inFlightFence, true, s_SwapChainTimeout));
    // wait for a whole minute at most
    constexpr uint64_t timeout = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::minutes(1)).count();
    auto [result, index]       = swapChain.acquireNextImage(timeout, *acqSemaphores.at(frameIndex));
    VK_CHECK(result);
    imageIndex = index;
    // Reset current fence
    renderer->device.resetFences(*inFlightFence);
}

void Impl::RecreateSwapChain()
{
    // Wait for GPU to finish work before destroying old swapchain resources
    renderer->device.waitIdle();

    // Fetch updated surface capabilities directly from the physical device
    vk::SurfaceCapabilitiesKHR capabilities = renderer->physicalDevice.getSurfaceCapabilitiesKHR(*surface);

    // Determine actual extent
    vk::Extent2D swapExtent = capabilities.currentExtent;

    // Handle minimized window (0x0 area on X11 / Wayland / Win32)
    if (swapExtent.width == 0 || swapExtent.height == 0) {
        return; // Pause rendering until window is restored
    }

    // Select Surface Format
    surfaceFormat = GetSurfaceFormat(renderer->physicalDevice, surface);

    // Populate SwapchainCreateInfo
    vk::SwapchainCreateInfoKHR createInfo { };
    createInfo.surface          = *surface;
    createInfo.minImageCount    = images.size();
    createInfo.imageFormat      = surfaceFormat.format;
    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
    createInfo.imageExtent      = swapExtent;
    createInfo.imageArrayLayers = 1;

    // Usage flags must match how you access swapchain images (e.g. blit destination)
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;

    createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    createInfo.preTransform     = capabilities.currentTransform;
    createInfo.compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode      = vk::PresentModeKHR::eFifo; // Guaranteed across drivers
    createInfo.clipped          = VK_TRUE;
    createInfo.oldSwapchain     = *swapChain; // Pass old handle for driver reuse

    // Create new RAII Swapchain & replace old handle
    vk::raii::SwapchainKHR newSwapChain(renderer->device, createInfo);
    swapChain = std::move(newSwapChain);

    // Update stored images and extent
    images = swapChain.getImages();
    extent = swapExtent;
}

void Impl::BlitImage(const RenderBuffer::Handle& a_RenderBuffer)
{
    auto& cmdBuffer       = presentCmdBuffers[frameIndex];
    auto& currentImage    = images[imageIndex];
    auto& renderSemaphore = renderCompleteSemaphores[imageIndex];
    auto& inFlightFence   = inFlightFences[frameIndex];
    auto& acqSemaphore    = acqSemaphores[frameIndex];
    cmdBuffer.reset();
    cmdBuffer.begin(vk::CommandBufferBeginInfo { vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
    VKImage::TransitionLayout(
        *cmdBuffer, **a_RenderBuffer, a_RenderBuffer->format,
        vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal,
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    VKImage::TransitionLayout(
        *cmdBuffer, currentImage, surfaceFormat.format,
        vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    {
        vk::ImageBlit blit;
        blit.srcSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
        blit.srcOffsets[1]  = vk::Offset3D(a_RenderBuffer->extent.width, a_RenderBuffer->extent.height, a_RenderBuffer->extent.depth);
        blit.dstSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
        blit.dstOffsets[1]  = vk::Offset3D(extent.width, extent.height, 1);
        cmdBuffer.blitImage(
            **a_RenderBuffer, vk::ImageLayout::eTransferSrcOptimal,
            currentImage, vk::ImageLayout::eTransferDstOptimal,
            { blit }, vk::Filter::eLinear);
    }
    VKImage::TransitionLayout(
        *cmdBuffer,
        **a_RenderBuffer,
        a_RenderBuffer->format,
        vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eGeneral,
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    VKImage::TransitionLayout(
        *cmdBuffer,
        currentImage,
        surfaceFormat.format,
        vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR,
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    cmdBuffer.end();
    // Submit blit operation
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eTransfer;
    vk::SubmitInfo submitInfo;
    submitInfo.setWaitDstStageMask(waitStage);
    submitInfo.setWaitSemaphores(*acqSemaphore);
    submitInfo.setSignalSemaphores(*renderSemaphore);
    submitInfo.setCommandBuffers(*cmdBuffer);
    renderer->transferQueue.submit(submitInfo, *inFlightFence);
}

void Impl::Present(const RenderBuffer::Handle& a_RenderBuffer)
{
    AcquireNextImage();
    BlitImage(a_RenderBuffer);
    // Present
    vk::PresentInfoKHR presentInfo;
    presentInfo.setWaitSemaphores(*renderCompleteSemaphores[imageIndex]);
    presentInfo.setSwapchains(*swapChain);
    presentInfo.setImageIndices(imageIndex);
    vk::Result result = renderer->presentQueue.presentKHR(presentInfo);
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        MSGErrorWarning("Swapchain suboptimal or out of date, recreating...");
        RecreateSwapChain();
    } else if (result != vk::Result::eSuccess) {
        VK_CHECK(result);
    }
    frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Impl::Wait()
{
    auto& inFlightFence = inFlightFences[frameIndex];
    if (inFlightFence.getStatus() == vk::Result::eNotReady)
        VK_INVOKE(renderer->device.waitForFences(*inFlightFence, true, s_SwapChainTimeout));
}
}

Msg::SwapChain::Handle Msg::SwapChain::Create(
    const Renderer::Handle& a_Renderer,
    const CreateSwapChainInfo& a_Info)
{
    return std::make_shared<Impl>(a_Renderer, a_Info);
}

Msg::SwapChain::Handle Msg::SwapChain::Recreate(
    const SwapChain::Handle& a_OldSwapChain,
    const CreateSwapChainInfo& a_Info)
{
    return std::make_shared<Impl>(a_OldSwapChain, a_Info);
}

void Msg::SwapChain::Present(
    const SwapChain::Handle& a_SwapChain,
    const RenderBuffer::Handle& a_RenderBuffer)
{
    a_SwapChain->Present(a_RenderBuffer);
}

void Msg::SwapChain::Wait(const SwapChain::Handle& a_SwapChain)
{
    a_SwapChain->Wait();
}
