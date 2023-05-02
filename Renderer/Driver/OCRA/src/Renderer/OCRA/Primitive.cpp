#include <Renderer/OCRA/Primitive.hpp>
#include <Renderer/OCRA/VertexBuffer.hpp>

#include <SG/Core/Primitive.hpp>

#include <OCRA/OCRA.hpp>

#include <glm/glm.hpp>

namespace TabGraph::Renderer {
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
    uint32_t _padding[5];
    inline static auto GetBindingDescriptions() {
        std::vector<OCRA::VertexBindingDescription> bindings(1);
        bindings.at(0).binding = 0;
        bindings.at(0).stride = sizeof(Vertex);
        bindings.at(0).inputRate = OCRA::VertexInputRate::Vertex;
        return bindings;
    }
    inline static auto GetAttributeDescription() {
        uint8_t location = 0;
        std::vector<OCRA::VertexAttributeDescription> attribs(10);
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(position)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, position);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(normal)::length();
        attribs.at(location).format.normalized = true;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, normal);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(tangent)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, tangent);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(texCoord_0)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, texCoord_0);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(texCoord_1)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, texCoord_1);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(texCoord_2)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, texCoord_2);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(texCoord_3)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, texCoord_3);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(color)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, color);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(joints)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Uint16;
        attribs.at(location).offset = offsetof(Vertex, joints);
        ++location;
        attribs.at(location).binding = 0;
        attribs.at(location).location = location;
        attribs.at(location).format.size = decltype(weights)::length();
        attribs.at(location).format.normalized = false;
        attribs.at(location).format.type = OCRA::VertexType::Float32;
        attribs.at(location).offset = offsetof(Vertex, weights);
        ++location;
        return attribs;
    }
};

template<unsigned L, typename T, bool Normalized = false>
glm::vec<L, T> ConvertData(const SG::BufferAccessor& a_Accessor, size_t a_Index)
{
    const auto componentNbr = a_Accessor.GetComponentNbr();
    glm::vec<L, T> ret{};
    for (auto i = 0u; i < L && i < componentNbr; ++i) {
        switch (a_Accessor.GetComponentType())
        {
        case SG::BufferAccessor::ComponentType::Int8:
            ret[i] = a_Accessor.GetComponent<glm::int8>(a_Index, i);
            break;
        case SG::BufferAccessor::ComponentType::Uint8:
            ret[i] = a_Accessor.GetComponent<glm::uint8>(a_Index, i);
            break;
        case SG::BufferAccessor::ComponentType::Int16:
            ret[i] = a_Accessor.GetComponent<glm::int16>(a_Index, i);
            break;
        case SG::BufferAccessor::ComponentType::Uint16:
            ret[i] = a_Accessor.GetComponent<glm::uint16>(a_Index, i);
            break;
        case SG::BufferAccessor::ComponentType::Float16:
            ret[i] = glm::detail::toFloat32(a_Accessor.GetComponent<glm::detail::hdata>(a_Index, i));
            break;
        case SG::BufferAccessor::ComponentType::Float32:
            ret[i] = a_Accessor.GetComponent<glm::f32>(a_Index, i);
            break;
        default:
            throw std::runtime_error("Unknown data format");
        }
    }
    if constexpr (Normalized) {
        if constexpr (L == 4) return glm::vec<L, T>(glm::normalize(glm::vec3(ret)), ret.w);
        else return glm::normalize(ret);
    }
    else return ret;
}

inline std::vector<Vertex> ConvertVertice(const SG::Primitive& a_Primitive)
{
    if (a_Primitive.GetPositions().empty()) throw std::runtime_error("No positions specified");
    std::vector<Vertex> vertice(a_Primitive.GetPositions().GetSize());
    auto hasPositions  = !a_Primitive.GetPositions().empty();
    auto hasNormals    = !a_Primitive.GetNormals().empty();
    auto hasTangent    = !a_Primitive.GetTangent().empty();
    auto hasTexCoord_0 = !a_Primitive.GetTexCoord0().empty();
    auto hasTexCoord_1 = !a_Primitive.GetTexCoord1().empty();
    auto hasTexCoord_2 = !a_Primitive.GetTexCoord2().empty();
    auto hasTexCoord_3 = !a_Primitive.GetTexCoord3().empty();
    auto hasColor      = !a_Primitive.GetColors().empty();
    auto hasJoints     = !a_Primitive.GetJoints().empty();
    auto hasWeights    = !a_Primitive.GetWeights().empty();
#ifdef _DEBUG
    assert(hasPositions);
    assert(hasNormals);
    if (hasJoints) assert(hasWeights);
#endif
    for (auto i = 0u; i < a_Primitive.GetPositions().GetSize(); ++i) {
        if (hasPositions)  vertice.at(i).position   = ConvertData<3, glm::f32>      (a_Primitive.GetPositions(), i);
        if (hasNormals)    vertice.at(i).normal     = ConvertData<3, glm::f32, true>(a_Primitive.GetNormals(),   i);
        if (hasTangent)    vertice.at(i).tangent    = ConvertData<4, glm::f32, true>(a_Primitive.GetTangent(),   i);
        if (hasTexCoord_0) vertice.at(i).texCoord_0 = ConvertData<2, glm::f32>      (a_Primitive.GetTexCoord0(), i);
        if (hasTexCoord_1) vertice.at(i).texCoord_1 = ConvertData<2, glm::f32>      (a_Primitive.GetTexCoord1(), i);
        if (hasTexCoord_2) vertice.at(i).texCoord_2 = ConvertData<2, glm::f32>      (a_Primitive.GetTexCoord2(), i);
        if (hasTexCoord_3) vertice.at(i).texCoord_3 = ConvertData<2, glm::f32>      (a_Primitive.GetTexCoord3(), i);
        if (hasColor)      vertice.at(i).color      = ConvertData<3, glm::f32>      (a_Primitive.GetColors(),    i);
        if (hasJoints)     vertice.at(i).joints     = ConvertData<4, glm::uint16>   (a_Primitive.GetJoints(),    i);
        if (hasWeights)    vertice.at(i).weights    = ConvertData<4, glm::f32>      (a_Primitive.GetWeights(),   i);
    }
    return vertice;
}

Primitive::Primitive(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::Device::Handle& a_Device, const SG::Primitive& a_Primitive)
    : vertexBuffer(a_PhysicalDevice, a_Device, ConvertVertice(a_Primitive))
{}
}
