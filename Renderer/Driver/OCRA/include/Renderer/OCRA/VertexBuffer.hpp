#pragma once

#include <Renderer/OCRA/MemoryHelper.hpp>

#include <OCRA/OCRA.hpp>

OCRA_DECLARE_HANDLE(OCRA::Memory);
OCRA_DECLARE_HANDLE(OCRA::Buffer);

namespace TabGraph::Renderer {
class VertexBuffer {
public:
    template <typename V>
    VertexBuffer(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::Device::Handle& a_Device, const std::vector<V>& a_Vertices)
        : size(sizeof(V) * a_Vertices.size())
        , vertexSize(sizeof(V))
        , memory(AllocateMemory(a_PhysicalDevice, a_Device, GetSize(), OCRA::MemoryPropertyFlagBits::HostVisible))
        , buffer(CreateBuffer(a_Device, memory, size, offset))
        , bindings(V::GetBindingDescriptions())
        , attribs(V::GetAttributeDescription())
    {
        FillMemory(memory, a_Vertices.data(), size, offset);
    }
    auto& GetBuffer() const { return buffer; }
    auto& GetMemory() const { return memory; }
    auto& GetOffset() const { return offset; }
    auto& GetSize() const { return size; }
    auto& GetVertexSize() const { return vertexSize; }
    auto GetVertexNbr() const { return GetSize() / GetVertexSize(); }
    auto& GetBindingDescriptions() const { return bindings; };
    auto& GetAttribsDescriptions() const { return attribs; }

private:
    const size_t size       = 0;
    const size_t offset     = 0;
    const size_t vertexSize = 0;
    const OCRA::Memory::Handle memory;
    const OCRA::Buffer::Handle buffer;
    const std::vector<OCRA::VertexBindingDescription> bindings;
    const std::vector<OCRA::VertexAttributeDescription> attribs;
};
}
