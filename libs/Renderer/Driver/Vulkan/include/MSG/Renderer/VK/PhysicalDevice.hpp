#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace Msg::Renderer {
constexpr auto s_QueueFlags = vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eTransfer;
uint32_t FindQueueFamily(
    const vk::raii::PhysicalDevice& a_PhysicalDevice,
    const vk::QueueFlags& a_QueueFlags);
vk::raii::Device CreateDevice(const vk::raii::PhysicalDevice& a_PhysicalDevice);
}
