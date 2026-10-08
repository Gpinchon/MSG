#pragma once

#include <memory>

#include <vulkan/vulkan_raii.hpp>

namespace Msg {
class VKMemoryAllocator;
}

namespace Msg {
class VKImage {
public:
    VKImage(
        const vk::raii::Device& a_Device,
        const vk::raii::PhysicalDevice& a_PhysicalDevice,
        const vk::ImageType& a_ImageType,
        const vk::Extent3D& a_Extent,
        const vk::Format& a_Format,
        const vk::ImageUsageFlags& a_Usage,
        VKMemoryAllocator& a_MemoryAllocator,
        const vk::MemoryPropertyFlags& a_MemoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
        const vk::ImageAspectFlags& a_AspectMask          = vk::ImageAspectFlagBits::eColor,
        const vk::ImageTiling& a_Tiling                   = vk::ImageTiling::eOptimal,
        const uint32_t& a_MipLevels                       = 1,
        const uint32_t& a_ArrayLayers                     = 1,
        const vk::SampleCountFlagBits& a_Samples          = vk::SampleCountFlagBits::e1);
    void TransitionLayout(
        const vk::CommandBuffer& a_CmdBuffer,
        const vk::ImageLayout& a_OldLayout,
        const vk::ImageLayout& a_NewLayout,
        const vk::ImageSubresourceRange& a_SubResource) const;
    static vk::PipelineStageFlags GetLayoutTransitionStageFlags(const vk::ImageLayout& a_Layout);
    static vk::AccessFlags GetLayoutTransitionAccessFlags(const vk::ImageLayout& a_Layout);
    static void TransitionLayout(
        const vk::CommandBuffer& a_CmdBuffer,
        const vk::Image& a_Image,
        const vk::Format& a_Format,
        const vk::ImageLayout& a_OldLayout,
        const vk::ImageLayout& a_NewLayout,
        const vk::ImageSubresourceRange& a_SubResource);
    vk::Extent3D extent;
    vk::Format format;
    uint32_t mipLevels;
    uint32_t arrayLayers;
    vk::raii::Image image;
    std::shared_ptr<vk::raii::DeviceMemory> memory;
};
}
