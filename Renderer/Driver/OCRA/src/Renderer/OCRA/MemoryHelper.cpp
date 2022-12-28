#include <OCRA/PhysicalDevice.hpp>
#include <OCRA/Memory.hpp>
#include <OCRA/Buffer.hpp>

#include <stdexcept>

namespace TabGraph::Renderer {
uint32_t FindProperMemoryType(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::PhysicalDevice::MemoryPropertyFlags& a_MemoryProperties)
{
    auto& memoryProperties = OCRA::PhysicalDevice::GetMemoryProperties(a_PhysicalDevice);
    for (auto memoryTypeIndex = 0u; memoryTypeIndex < memoryProperties.memoryTypes.size(); ++memoryTypeIndex) {
        if (memoryProperties.memoryTypes.at(memoryTypeIndex).propertyFlags == a_MemoryProperties)
            return memoryTypeIndex;
    }
    //Couldn't find optimal memory type, take any fitting type
    for (auto memoryTypeIndex = 0u; memoryTypeIndex < memoryProperties.memoryTypes.size(); ++memoryTypeIndex) {
        if ((memoryProperties.memoryTypes.at(memoryTypeIndex).propertyFlags & a_MemoryProperties) != 0)
            return memoryTypeIndex;
    }
    throw std::runtime_error("Could not find matching memory type");
    return (std::numeric_limits<uint32_t>::max)();
}

OCRA::Memory::Handle AllocateMemory(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::Device::Handle& a_Device, const uint64_t& a_Size, const OCRA::PhysicalDevice::MemoryPropertyFlags& a_MemoryProperties)
{
    OCRA::Memory::Info memoryInfo;
    memoryInfo.memoryTypeIndex = FindProperMemoryType(a_PhysicalDevice, a_MemoryProperties);
    memoryInfo.size = a_Size;
    return OCRA::Memory::Allocate(a_Device, memoryInfo);
}

OCRA::Buffer::Handle CreateBuffer(const OCRA::Device::Handle& a_Device, const OCRA::Memory::Handle& a_Memory, size_t a_Size, size_t a_Offset) {
    OCRA::Buffer::Info info;
    info.size = a_Size;
    info.usage = OCRA::Buffer::UsageFlagBits::VertexBuffer;
    auto buffer = OCRA::Buffer::Create(a_Device, info);
    OCRA::Buffer::BindMemory(a_Device, buffer, a_Memory, a_Offset);
    return buffer;
}

void FillMemory(const OCRA::Device::Handle& a_Device, const OCRA::Memory::Handle& a_Memory, const void* a_Data, size_t a_Size, size_t a_Offset) {
    OCRA::Memory::MappedRange range;
    range.memory = a_Memory;
    range.length = a_Size;
    range.offset = a_Offset;
    auto bufferPtr = OCRA::Memory::Map(a_Device, range);
    std::memcpy(bufferPtr, a_Data, a_Size);
    OCRA::Memory::Unmap(a_Device, a_Memory);
}
}