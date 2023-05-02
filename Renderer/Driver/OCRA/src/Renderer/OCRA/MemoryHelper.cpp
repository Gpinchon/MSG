#include <OCRA/OCRA.hpp>

#include <stdexcept>

namespace TabGraph::Renderer {
OCRA::Memory::Handle AllocateMemory(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::Device::Handle& a_Device, const uint64_t& a_Size, const OCRA::MemoryPropertyFlags& a_MemoryProperties)
{
    OCRA::AllocateMemoryInfo memoryInfo;
    memoryInfo.memoryTypeIndex = OCRA::PhysicalDevice::FindMemoryType(a_PhysicalDevice, a_MemoryProperties);
    memoryInfo.size = a_Size;
    return OCRA::Device::AllocateMemory(a_Device, memoryInfo);
}

OCRA::Buffer::Handle CreateBuffer(const OCRA::Device::Handle& a_Device, const OCRA::Memory::Handle& a_Memory, size_t a_Size, size_t a_Offset) {
    OCRA::CreateBufferInfo info;
    info.size = a_Size;
    info.usage = OCRA::BufferUsageFlagBits::VertexBuffer;
    auto buffer = OCRA::Device::CreateBuffer(a_Device, info);
    OCRA::Buffer::BindMemory(buffer, a_Memory, a_Offset);
    return buffer;
}

void FillMemory(const OCRA::Memory::Handle& a_Memory, const void* a_Data, size_t a_Size, size_t a_Offset) {
    OCRA::MemoryMappedRange range;
    range.memory = a_Memory;
    range.length = a_Size;
    range.offset = a_Offset;
    auto bufferPtr = OCRA::Memory::Map(range);
    std::memcpy(bufferPtr, a_Data, a_Size);
    OCRA::Memory::Unmap(a_Memory);
}
}