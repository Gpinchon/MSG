#pragma once

#include <cstddef>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

namespace Msg {
class VKMemoryAllocator;
}

namespace Msg {
class VKBuffer {
public:
    /**
     * @brief creates a buffer from specified memory
     * @arg a_Offset, byte offset inside the specified memory
     * @arg a_Size, byte size inside the specified memory
     * @arg a_Memory, the memory to use for this buffer
     */
    VKBuffer(
        const vk::raii::Device& a_Device, const vk::BufferUsageFlags& a_Usage,
        const size_t& a_ByteOffset, const size_t& a_ByteSize,
        const std::shared_ptr<vk::raii::DeviceMemory>& a_Memory);
    VKBuffer(
        const vk::raii::Device& a_Device, const vk::BufferUsageFlags& a_Usage,
        const size_t& a_ByteOffset, const size_t& a_ByteSize,
        VKMemoryAllocator& a_MemoryAllocator, const vk::MemoryPropertyFlags& a_MemoryProperties);
    void* Map(const std::size_t a_Offset = 0, const size_t& a_Size = VK_WHOLE_SIZE);
    void Unmap();
    size_t byteOffset = 0; // the byte offset inside memory
    size_t byteSize;
    vk::raii::Buffer buffer;
    std::shared_ptr<vk::raii::DeviceMemory> memory;
    void* mapped = nullptr;
};
}