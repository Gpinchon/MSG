#pragma once

#include <Renderer/OCRA/Renderer.hpp>

#include <SG/Core/Primitive.hpp>
#include <SG/Component/Mesh.hpp>

#include <OCRA/Buffer.hpp>
#include <OCRA/Shader/Stage.hpp>
#include <OCRA/Memory.hpp>
#include <OCRA/Pipeline/VertexInputState.hpp>
#include <OCRA/Pipeline/InputAssemblyState.hpp>

#include <vector>

namespace TabGraph::Renderer {
template<typename Factory>
struct LazyConstructor {
    using result_type = std::invoke_result_t<const Factory&>;
    constexpr LazyConstructor(Factory&& a_Factory) : factory(a_Factory) {}
    constexpr operator result_type() const noexcept(std::is_nothrow_invocable_v<const Factory&>) {
        return factory();
    }
    const Factory factory;
};

inline uint32_t FindProperMemoryType(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::PhysicalDevice::MemoryPropertyFlags& a_MemoryProperties)
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

inline OCRA::Memory::Handle AllocateMemory(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::Device::Handle& a_Device, const uint64_t& a_Size, const OCRA::PhysicalDevice::MemoryPropertyFlags& a_MemoryProperties)
{
    OCRA::Memory::Info memoryInfo;
    memoryInfo.memoryTypeIndex = FindProperMemoryType(a_PhysicalDevice, a_MemoryProperties);
    memoryInfo.size = a_Size;
    return OCRA::Memory::Allocate(a_Device, memoryInfo);
}

struct Vertex {
    glm::vec3    position;
    glm::vec3    normal;
    glm::vec4    tangent;
    glm::vec2    texCoord_0;
    glm::vec2    texCoord_1;
    glm::vec2    texCoord_2;
    glm::vec2    texCoord_3;
    glm::vec3    color;
    glm::u16vec4 joints;
    glm::vec4    weights;
    inline static auto GetBindingDescriptions() {
        std::vector<OCRA::Pipeline::VertexInputState::BindingDescription> bindings(1);
        bindings.at(0).binding = 0;
        bindings.at(0).stride = sizeof(Vertex);
        bindings.at(0).inputRate = OCRA::Pipeline::VertexInputState::BindingDescription::InputRate::Vertex;
        return bindings;
    }
    inline static auto GetAttributeDescription() {
        std::vector<OCRA::Pipeline::VertexInputState::AttributeDescription> attribs(3);
        attribs.at(0).binding = 0;
        attribs.at(0).location = 0;
        attribs.at(0).format.size = decltype(position)::length();
        attribs.at(0).format.normalized = false;
        attribs.at(0).format.type = OCRA::VertexType::Float32;
        attribs.at(0).offset = offsetof(Vertex, position);

        attribs.at(1).binding = 0;
        attribs.at(1).location = 1;
        attribs.at(1).format.size = decltype(normal)::length();
        attribs.at(1).format.normalized = true;
        attribs.at(1).format.type = OCRA::VertexType::Float32;
        attribs.at(1).offset = offsetof(Vertex, normal);

        attribs.at(2).binding = 0;
        attribs.at(2).location = 2;
        attribs.at(2).format.size = decltype(tangent)::length();
        attribs.at(2).format.normalized = false;
        attribs.at(2).format.type = OCRA::VertexType::Float32;
        attribs.at(2).offset = offsetof(Vertex, tangent);
        return attribs;
    }
};

class VertexBuffer {
public:
    template<typename V = Vertex>
    VertexBuffer(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::Device::Handle& a_Device, const std::vector<V>& a_Vertices)
        : size(sizeof(V)* a_Vertices.size())
        , vertexSize(sizeof(V))
        , memory(AllocateMemory(a_PhysicalDevice, a_Device, GetSize(), OCRA::PhysicalDevice::MemoryPropertyFlagBits::HostVisible))
        , buffer(CreateBuffer(a_Device))
        , bindings(V::GetBindingDescriptions())
        , attribs(V::GetAttributeDescription())
    {
        OCRA::Memory::MappedRange range;
        range.memory = memory;
        range.length = GetSize();
        range.offset = GetOffset();
        auto bufferPtr = OCRA::Memory::Map(a_Device, range);
        std::memcpy(bufferPtr, a_Vertices.data(), GetSize());
        OCRA::Memory::Unmap(a_Device, memory);
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
    OCRA::Buffer::Handle CreateBuffer(const OCRA::Device::Handle& a_Device) {
        OCRA::Buffer::Info info;
        info.size = GetSize();
        info.usage = OCRA::Buffer::UsageFlagBits::VertexBuffer;
        auto buffer = OCRA::Buffer::Create(a_Device, info);
        OCRA::Buffer::BindMemory(a_Device, buffer, memory, GetOffset());
        return buffer;
    }
    const size_t offset{ 0 };
    const size_t size{ 0 };
    const size_t vertexSize{ 0 };
    const OCRA::Memory::Handle memory;
    const OCRA::Buffer::Handle buffer;
    const std::vector<OCRA::Pipeline::VertexInputState::BindingDescription>   bindings;
    const std::vector<OCRA::Pipeline::VertexInputState::AttributeDescription> attribs;
};

std::vector<Vertex> ConvertVertice(const SG::Primitive& a_Primitive)
{
    if (a_Primitive.GetPositions().empty()) throw std::runtime_error("No positions specified");
    std::vector<Vertex> vertice(a_Primitive.GetPositions().GetSize());
    for (auto i = 0u; i < a_Primitive.GetPositions().GetSize(); ++i) {
        vertice.at(i).position = a_Primitive.GetPositions().at<glm::vec3>(i);
    }
    return vertice;
}

struct Primitive {
    Primitive(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::Device::Handle& a_Device, const SG::Primitive& a_Primitive)
        : vertexBuffer(a_PhysicalDevice, a_Device, ConvertVertice(a_Primitive))
    {}
    VertexBuffer                vertexBuffer;
	OCRA::Buffer::Handle        indices;
    OCRA::Shader::Stage::Handle vertexShader;
};
struct Material {

};
}

namespace TabGraph::Renderer::Component {
struct MeshData {
    MeshData(
        const Renderer::Handle& a_Renderer,
        const SG::Component::Mesh& a_Mesh)
    {
        for (const auto& it : a_Mesh.primitives) {
            const auto& primitive = it.first;
            auto [it, success] = a_Renderer->primitives.try_emplace(primitive, LazyConstructor([=] {
                return std::make_shared<Primitive>(a_Renderer->physicalDevice, a_Renderer->logicalDevice, *primitive);
            }));
            primitives.push_back(it->second);
        }
    }
    std::vector<std::shared_ptr<Renderer::Primitive>> primitives;
};
}