#include <cstddef>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

namespace Msg {
class VKMemoryAllocator {
public:
    VKMemoryAllocator(
        const vk::raii::Device& a_Device,
        const vk::raii::PhysicalDevice& a_PhysicalDevice)
        : device(a_Device)
        , physicalDevice(a_PhysicalDevice)
    {
    }
    std::shared_ptr<vk::raii::DeviceMemory> AllocateMemory(
        const vk::MemoryRequirements& a_MemRequirements,
        const vk::MemoryPropertyFlags& a_Properties);
    const vk::raii::Device& device;
    const vk::raii::PhysicalDevice& physicalDevice;
    size_t allocated = 0; // the total amount of memory allocated, decreases when device memory is destroyed
};
}