#pragma once

#include <Renderer/OCRA/MemoryHelper.hpp>
#include <Renderer/OCRA/AttributeBuffer.hpp>

#include <OCRA/OCRA.hpp>

namespace TabGraph::Renderer {
class VertexBuffer : public AttributeBuffer {
public:
    template <typename V>
    VertexBuffer(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::Device::Handle& a_Device, const std::vector<V>& a_Vertices)
        : AttributeBuffer(a_PhysicalDevice, a_Device, a_Vertices)
        , vertexInput({ 0, V::GetAttributeDescription(), V::GetBindingDescriptions() })
    {}
    auto& GetVertexSize() const { return GetAttribSize(); }
    auto GetVertexNbr() const { return GetSize() / GetVertexSize(); }
    auto& GetVertexInput() const { return vertexInput; }

private:
    const OCRA::PipelineVertexInputState vertexInput;
};
}
