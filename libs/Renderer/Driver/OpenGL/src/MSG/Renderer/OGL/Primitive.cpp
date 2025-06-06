#include <MSG/Debug.hpp>
#include <MSG/Mesh/Primitive.hpp>
#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLVertexArray.hpp>
#include <MSG/Renderer/OGL/Primitive.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/ToGL.hpp>
#include <MSG/Renderer/OGL/Vertex.hpp>

#include <GL/glew.h>
#include <stdexcept>

namespace MSG::Renderer {
template <unsigned L, typename T>
static inline glm::vec<L, T> ConvertData(const BufferAccessor& a_Accessor, size_t a_Index)
{
    const auto componentNbr = a_Accessor.GetComponentNbr();
    glm::vec<L, T> ret {};
    for (auto i = 0u; i < L && i < componentNbr; ++i) {
        switch (a_Accessor.GetComponentType()) {
        case Core::DataType::Int8:
            ret[i] = T(a_Accessor.template GetComponent<glm::int8>(a_Index, i));
            break;
        case Core::DataType::Uint8:
            ret[i] = T(a_Accessor.template GetComponent<glm::uint8>(a_Index, i));
            break;
        case Core::DataType::Int16:
            ret[i] = T(a_Accessor.template GetComponent<glm::int16>(a_Index, i));
            break;
        case Core::DataType::Uint16:
            ret[i] = T(a_Accessor.template GetComponent<glm::uint16>(a_Index, i));
            break;
        case Core::DataType::Int32:
            ret[i] = T(a_Accessor.template GetComponent<glm::int32>(a_Index, i));
            break;
        case Core::DataType::Uint32:
            ret[i] = T(a_Accessor.template GetComponent<glm::uint32>(a_Index, i));
            break;
        case Core::DataType::Float16:
            ret[i] = T(glm::detail::toFloat32(a_Accessor.template GetComponent<glm::detail::hdata>(a_Index, i)));
            break;
        case Core::DataType::Float32:
            ret[i] = T(a_Accessor.template GetComponent<glm::f32>(a_Index, i));
            break;
        default:
            throw std::runtime_error("Unknown data format");
        }
    }
    return ret;
}

inline std::vector<Vertex> ConvertVertice(const MeshPrimitive& a_Primitive)
{
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
    if (!hasPositions)
        debugLog("Primitive has no positions, nothing might be displayed");
    if (!hasNormals)
        debugLog("Primitive has no normals, lighting might be broken");
    if (!hasTangent)
        debugLog("Primitive has no tangents, lighting might be broken");
    if (!hasTexCoord_0 && !hasTexCoord_1 && !hasTexCoord_2 && !hasTexCoord_3)
        debugLog("Primitive has no texture coordinates, texturing might be broken");
    if (!hasColor)
        debugLog("Primitive has no color, surface might be black");
    if (hasJoints && !hasWeights)
        debugLog("Primitive has Joints but no Weights, skinning might be broken");
    for (auto i = 0u; i < a_Primitive.GetPositions().GetSize(); ++i) {
        if (hasPositions)
            vertice.at(i).position = ConvertData<3, glm::f32>(a_Primitive.GetPositions(), i);
        if (hasNormals) {
            auto normal          = ConvertData<3, glm::f32>(a_Primitive.GetNormals(), i);
            vertice.at(i).normal = glm::normalize(normal);
        }
        if (hasTangent) {
            auto tangent          = ConvertData<4, glm::f32>(a_Primitive.GetTangent(), i);
            vertice.at(i).tangent = glm::vec4(glm::normalize(glm::vec3(tangent)), glm::sign(tangent.w));
        }
        if (hasTexCoord_0)
            vertice.at(i).texCoord[0] = ConvertData<2, glm::f32>(a_Primitive.GetTexCoord0(), i);
        if (hasTexCoord_1)
            vertice.at(i).texCoord[1] = ConvertData<2, glm::f32>(a_Primitive.GetTexCoord1(), i);
        if (hasTexCoord_2)
            vertice.at(i).texCoord[2] = ConvertData<2, glm::f32>(a_Primitive.GetTexCoord2(), i);
        if (hasTexCoord_3)
            vertice.at(i).texCoord[3] = ConvertData<2, glm::f32>(a_Primitive.GetTexCoord3(), i);
        if (hasColor) {
            auto color          = ConvertData<3, glm::f32>(a_Primitive.GetColors(), i);
            vertice.at(i).color = glm::clamp(color, 0.f, 1.f);
        }
        if (hasJoints)
            vertice.at(i).joints = ConvertData<4, glm::f32>(a_Primitive.GetJoints(), i);
        if (hasWeights)
            vertice.at(i).weights = ConvertData<4, glm::f32>(a_Primitive.GetWeights(), i);
    }
    return vertice;
}

inline std::vector<unsigned> ConvertIndice(const MeshPrimitive& a_Primitive)
{
    std::vector<unsigned> indice(a_Primitive.GetIndices().GetSize());
    if (a_Primitive.GetIndices().empty())
        debugLog("Primitive is not indexed");
    for (auto i = 0u; i < a_Primitive.GetIndices().GetSize(); ++i) {
        indice.at(i) = ConvertData<1, glm::uint32>(a_Primitive.GetIndices(), i).x;
    }
    return indice;
}

Primitive::Primitive(OGLContext& a_Context, MeshPrimitive& a_Primitive)
    : drawMode(ToGL(a_Primitive.GetDrawingMode()))
{
    constexpr auto attribsDesc = Vertex::GetAttributeDescription();
    auto vertice               = ConvertVertice(a_Primitive);
    auto vertexBuffer          = std::make_shared<OGLBuffer>(a_Context, vertice.size() * sizeof(Vertex), vertice.data(), 0);

    OGLVertexBindingDescription binding;
    binding.buffer = vertexBuffer;
    binding.index  = 0;
    binding.offset = 0;
    binding.stride = sizeof(Vertex);

    std::vector<OGLVertexAttributeDescription> attribs(attribsDesc.begin(), attribsDesc.end());
    std::vector<OGLVertexBindingDescription> bindings { binding };

    auto indice = ConvertIndice(a_Primitive);
    if (!indice.empty()) {
        auto indexBuffer              = std::make_shared<OGLBuffer>(a_Context,
                         indice.size() * sizeof(unsigned), indice.data(), 0);
        OGLIndexDescription indexDesc = {};
        indexDesc.type                = GL_UNSIGNED_INT;
        vertexArray                   = std::make_shared<OGLVertexArray>(a_Context,
                              uint32_t(vertice.size()), attribs, bindings,
                              uint32_t(indice.size()), indexDesc, indexBuffer);
    } else {
        vertexArray = std::make_shared<OGLVertexArray>(a_Context,
            uint32_t(vertice.size()), attribs, bindings);
    }
}
}
