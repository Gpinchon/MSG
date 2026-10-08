#include <MSG/PixelDescriptor.hpp>
#include <MSG/Renderer/RenderBuffer.hpp>
#include <MSG/Debug.hpp>
#include <MSG/SwapChain/SwapChain.hpp>

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan_raii.hpp>
#elif __linux__
#define VK_USE_PLATFORM_XLIB_KHR
#include <vulkan/vulkan_raii.hpp>
#endif

#include <MSG/VKInvoke.hpp>
#include <MSG/Renderer/VK/RenderBuffer.hpp>
#include <MSG/Renderer/VK/Renderer.hpp>
#include <MSG/SwapChain/VK/SwapChain.hpp>

namespace Msg::SwapChain {
// wait for a whole second at most
constexpr uint64_t s_SwapChainTimeout = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();

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

bool PresentModeSupported(const std::vector<vk::PresentModeKHR>& a_PresentModes, const vk::PresentModeKHR& a_Expected)
{
    for (auto& presentMode : a_PresentModes) {
        if (presentMode == a_Expected)
            return true;
    }
    return false;
}

/** @brief Tries to find a compatible present mode.
 * Will try a_Expected -> Mailbox -> Immediate -> Fifo.
 * Throws a fatal error if none could be found.
 */
vk::PresentModeKHR GetPresentMode(const vk::PhysicalDevice& a_PhysicalDevice, const vk::SurfaceKHR& a_Surface, const vk::PresentModeKHR& a_Expected)
{
    auto presentModes = a_PhysicalDevice.getSurfacePresentModesKHR(a_Surface);
    if (PresentModeSupported(presentModes, a_Expected))
        return a_Expected;
    else if (PresentModeSupported(presentModes, vk::PresentModeKHR::eMailbox)) {
        MSGErrorWarning("Requested present mode unsupported, defaulting to Mailbox");
        return vk::PresentModeKHR::eMailbox;
    } else if (PresentModeSupported(presentModes, vk::PresentModeKHR::eImmediate)) {
        MSGErrorWarning("Requested present mode unsupported, defaulting to Immediate");
        return vk::PresentModeKHR::eImmediate;
    } else if (PresentModeSupported(presentModes, vk::PresentModeKHR::eFifo)) {
        MSGErrorWarning("Requested present mode unsupported, defaulting to Fifo");
        return vk::PresentModeKHR::eFifo;
    }
    MSGErrorFatal("Could not find a supported present mode !");
    return vk::PresentModeKHR(-1); // return incorrect present mode on purpose
}

vk::SurfaceFormatKHR GetSurfaceFormat(const vk::raii::PhysicalDevice& a_PhysicalDevice, const vk::raii::SurfaceKHR& a_Surface)
{
    for (auto& format : a_PhysicalDevice.getSurfaceFormatsKHR(*a_Surface)) {
        if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            return format;
    }
    MSGErrorFatal("Could not find proper surface format !");
}

vk::raii::SurfaceKHR CreateSurface(
    vk::raii::Instance& a_Instance,
    const Msg::SwapChain::CreateSwapChainInfo& a_Info)
{
#ifdef _WIN32
    vk::Win32SurfaceCreateInfoKHR vkInfo(
        vk::Win32SurfaceCreateFlagsKHR { },
        (HINSTANCE)GetWindowLongPtr(vkInfo.hwnd, GWLP_HINSTANCE),
        WindowFromDC(std::any_cast<HDC>(a_Info.windowInfo.nativeDisplayHandle)));
    return a_Instance.createWin32SurfaceKHR(vkInfo);
#elif __linux__
    vk::XlibSurfaceCreateInfoKHR vkInfo(
        vk::XlibSurfaceCreateFlagsKHR { },
        std::any_cast<Display*>(a_Info.windowInfo.nativeDisplayHandle),
        std::any_cast<Window>(a_Info.windowInfo.nativeWindowHandle));
    return a_Instance.createXlibSurfaceKHR(vkInfo);
#endif //_WIN32
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
    const vk::raii::SurfaceKHR& a_Surface,
    const vk::SurfaceFormatKHR& a_SurfaceFormat,
    const uint32_t& a_ImageCount,
    const vk::Extent2D a_Extent,
    const vk::PresentModeKHR& a_PresentMode,
    const vk::SwapchainKHR& a_OldSwapChain = nullptr)
{
    vk::SwapchainCreateInfoKHR info;
    info.setImageFormat(a_SurfaceFormat.format);
    info.setImageColorSpace(a_SurfaceFormat.colorSpace);
    info.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst);
    info.setImageSharingMode(vk::SharingMode::eExclusive);
    info.setImageExtent(a_Extent);
    info.setImageArrayLayers(1);
    info.setMinImageCount(a_ImageCount);
    info.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
    info.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);
    info.setPresentMode(a_PresentMode);
    info.setSurface(*a_Surface);
    info.setOldSwapchain(a_OldSwapChain);
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
    , surface(CreateSurface(renderer->instance, a_Info))
    , presentMode(GetPresentMode(renderer->physicalDevice, surface, GetVKPresentMode(a_Info.presentMode)))
    , surfaceFormat(GetSurfaceFormat(renderer->physicalDevice, surface))
    , swapChain(renderer->device, GetVKCreateInfo(surface, surfaceFormat, a_Info.imageCount, { a_Info.width, a_Info.height }, presentMode))
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
    , presentMode(GetPresentMode(renderer->physicalDevice, surface, a_OldSwapChain->presentMode))
    , surfaceFormat(GetSurfaceFormat(renderer->physicalDevice, surface))
    , swapChain(renderer->device, GetVKCreateInfo(surface, surfaceFormat, a_Info.imageCount, { a_Info.width, a_Info.height }, presentMode, a_OldSwapChain->swapChain))
    , images(swapChain.getImages())
    , extent(a_Info.width, a_Info.height)
    , acqSemaphores(std::move(a_OldSwapChain->acqSemaphores))
    , inFlightFences(std::move(a_OldSwapChain->inFlightFences))
    , presentCmdBuffers(std::move(a_OldSwapChain->presentCmdBuffers))
    , renderCompleteSemaphores(CreateSemaphores(renderer->device, images.size()))
{
}

Impl::~Impl()
{
    Wait();
    renderer->device.waitIdle();
    MSGDebugLog("Swapchain instance destroyed.");
}

void Impl::AcquireNextImage()
{
    auto& inFlightFence = inFlightFences[frameIndex];
    // Wait for previous blit operation to be done
    VK_INVOKE(renderer->device.waitForFences(*inFlightFence, true, s_SwapChainTimeout));
    auto [result, index] = swapChain.acquireNextImage(s_SwapChainTimeout, *acqSemaphores.at(frameIndex));
    VK_CHECK(result);
    imageIndex = index;
    // Reset current fence after acquiring image
    renderer->device.resetFences(*inFlightFence);
}

void Impl::RecreateSwapChain()
{
    // Wait for GPU to finish work
    renderer->device.waitIdle();
    // Fetch updated surface capabilities
    vk::SurfaceCapabilitiesKHR capabilities = renderer->physicalDevice.getSurfaceCapabilitiesKHR(*surface);
    if (capabilities.currentExtent.width == 0 || capabilities.currentExtent.height == 0)
        return; // early bail, incorrect extent (window might be minimized)
    else if (capabilities.currentExtent.width != 0xFFFFFFFF && capabilities.currentExtent.height != 0xFFFFFFFF) {
        extent        = capabilities.currentExtent; // only change extent if surface size is not determined by the extent of the swapchain
        extent.width  = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }
    surfaceFormat = GetSurfaceFormat(renderer->physicalDevice, surface);
    presentMode   = GetPresentMode(renderer->physicalDevice, surface, presentMode);
    swapChain     = vk::raii::SwapchainKHR(renderer->device, GetVKCreateInfo(surface, surfaceFormat, images.size(), extent, presentMode, swapChain));
    images        = swapChain.getImages();
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
    a_RenderBuffer->TransitionLayout(
        *cmdBuffer,
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
            *a_RenderBuffer->image, vk::ImageLayout::eTransferSrcOptimal,
            currentImage, vk::ImageLayout::eTransferDstOptimal,
            { blit }, vk::Filter::eLinear);
    }
    a_RenderBuffer->TransitionLayout(
        *cmdBuffer,
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
