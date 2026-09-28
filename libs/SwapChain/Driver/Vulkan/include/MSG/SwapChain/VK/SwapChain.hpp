#pragma once

#include <MSG/Renderer/Handles.hpp>
#include <MSG/SwapChain/Structs.hpp>
#include <MSG/SwapChain/SwapChain.hpp>
#include <MSG/SwapChain/VK/Surface.hpp>
#include <MSG/VKImage.hpp>

#include <vulkan/vulkan_raii.hpp>

namespace Msg::SwapChain {
class Impl {
public:
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2; // Fixed CPU-GPU ring buffer
    Impl(
        const Renderer::Handle& a_Renderer,
        const CreateSwapChainInfo& a_Info);
    Impl(
        const SwapChain::Handle& a_OldSwapChain,
        const CreateSwapChainInfo& a_Info);
    ~Impl();
    void AcquireNextImage();
    void RecreateSwapChain();
    void BlitImage(const RenderBuffer::Handle& a_RenderBuffer);
    void Present(const RenderBuffer::Handle& a_RenderBuffer);
    void Wait();
    Renderer::Handle renderer;
    Surface surface;
    vk::SurfaceFormatKHR surfaceFormat;
    vk::raii::SwapchainKHR swapChain;
    std::vector<vk::Image> images;
    vk::Extent2D extent;
    // Per-Frame-In-Flight Sync (Size = 2)
    std::vector<vk::raii::Semaphore> acqSemaphores;
    std::vector<vk::raii::Fence> inFlightFences;
    // Per-Swapchain-Image Sync (Size = images.size())
    std::vector<vk::raii::Semaphore> renderCompleteSemaphores;
    std::vector<vk::raii::CommandBuffer> presentCmdBuffers;
    uint32_t imageIndex = 0;
    uint32_t frameIndex = 0;
};
}
