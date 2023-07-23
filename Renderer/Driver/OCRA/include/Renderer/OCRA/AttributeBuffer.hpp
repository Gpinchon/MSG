#pragma once

#include <Renderer/OCRA/MemoryHelper.hpp>

#include <OCRA/Handle.hpp>

namespace TabGraph::Renderer {
class AttributeBuffer {
public:
    /**
     * @brief Creates an AttributeBuffer from preallocated Memory
     */
    AttributeBuffer(
        const OCRA::BufferUsageFlags& a_Usage,
        const OCRA::Device::Handle& a_Device,
        const size_t& a_Count,
        const size_t& a_Offset,
        const size_t& a_AttribSize,
        const OCRA::Memory::Handle& a_Memory)
        : count(a_Count)
        , offset(a_Offset)
        , attribSize(a_AttribSize)
        , memory(a_Memory)
        , buffer(CreateBuffer(a_Usage, a_Device, memory, count * attribSize, offset))
    {
    }
    /**
     * @brief Creates an AttributeBuffer from a_Data, allocates Memory
     */
    AttributeBuffer(
        const OCRA::BufferUsageFlags& a_Usage,
        const OCRA::PhysicalDevice::Handle& a_PhysicalDevice,
        const OCRA::Device::Handle& a_Device,
        const size_t& a_AttributeSize,
        const size_t& a_Count,
        const std::byte* a_Data)
        : AttributeBuffer(a_Usage, a_Device, a_Count, 0, a_AttributeSize, AllocateMemory(a_PhysicalDevice, a_Device, a_AttributeSize * a_Count, OCRA::MemoryPropertyFlagBits::HostVisible))
    {
        FillMemory(memory, a_Data, count * attribSize, offset);
    }
    /**
     * @brief Creates an AttributeBuffer from a_Data vector, allocates Memory
     */
    template <typename T>
    AttributeBuffer(
        const OCRA::BufferUsageFlags& a_Usage,
        const OCRA::PhysicalDevice::Handle& a_PhysicalDevice,
        const OCRA::Device::Handle& a_Device,
        const std::vector<T>& a_Data)
        : AttributeBuffer(a_Usage, a_PhysicalDevice, a_Device, sizeof(T), a_Data.size(), reinterpret_cast<const std::byte*>(a_Data.data()))
    {
    }
    auto& GetBuffer() const { return buffer; }
    auto& GetMemory() const { return memory; }
    /**
     * @brief offset of the attribute inside memory
     */
    auto& GetOffset() const { return offset; }
    /*
     * @brief Number of vertice or indice
     **/
    auto& GetCount() const { return count; }
    /**
     * @brief Attrib size in bytes sizeof(vec3) for instance
     */
    auto& GetAttribSize() const { return attribSize; }
    /**
     * @brief Total size in bytes (count * attribSize)
     */
    auto GetSize() const { return GetCount() * GetAttribSize(); }

private:
    const size_t count      = 0;
    const size_t offset     = 0;
    const size_t attribSize = 0;
    const OCRA::Memory::Handle memory;
    const OCRA::Buffer::Handle buffer;
};
}
