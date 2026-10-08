#include <MSG/VKBuffer.hpp>
#include <MSG/VKMemoryAllocator.hpp>
#include <MSG/Debug.hpp>

vk::BufferCreateInfo GetBufferCreateInfo(const size_t& a_Size, const vk::BufferUsageFlags a_Usage, const vk::SharingMode& a_SharingMode)
{
    return vk::BufferCreateInfo({ }, a_Size, a_Usage, a_SharingMode);
}

Msg::VKBuffer::VKBuffer(
    const vk::raii::Device& a_Device, const vk::BufferUsageFlags& a_Usage,
    const size_t& a_Offset, const size_t& a_Size,
    const std::shared_ptr<vk::raii::DeviceMemory>& a_Memory)
    : byteOffset(a_Offset)
    , byteSize(a_Size)
    , buffer(a_Device, vk::BufferCreateInfo({ }, a_Size, a_Usage, vk::SharingMode::eExclusive))
    , memory(a_Memory)
{
    buffer.bindMemory(*memory, byteOffset);
}

Msg::VKBuffer::VKBuffer(
    const vk::raii::Device& a_Device, const vk::BufferUsageFlags& a_Usage,
    const size_t& a_Offset, const size_t& a_Size,
    VKMemoryAllocator& a_MemoryAllocator, const vk::MemoryPropertyFlags& a_MemoryProperties)
    : byteOffset(a_Offset)
    , byteSize(a_Size)
    , buffer(a_Device, vk::BufferCreateInfo({ }, a_Size, a_Usage, vk::SharingMode::eExclusive))
    , memory(a_MemoryAllocator.AllocateMemory(buffer.getMemoryRequirements(), a_MemoryProperties))
{
    buffer.bindMemory(*memory, byteOffset);
}

void* Msg::VKBuffer::Map(const std::size_t a_Offset, const size_t& a_Size)
{
    MSGCheckErrorFatal((a_Offset + a_Size) > byteSize, "Mapped range exceeds memory size !");
    MSGCheckErrorFatal(mapped == nullptr, "Memory already mapped !"); // should be catched by validation layer in debug if memory is already mapped
    return mapped = memory->mapMemory(a_Offset, a_Size);
}

void Msg::VKBuffer::Unmap()
{
    if (mapped != nullptr) {
        memory->unmapMemory();
        mapped = nullptr;
    }
}