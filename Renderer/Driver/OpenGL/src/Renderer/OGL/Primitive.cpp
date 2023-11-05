#include <Renderer/OGL/Primitive.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/Renderer.hpp>
#include <Renderer/OGL/Vertex.hpp>

#include <SG/Core/Primitive.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer {
static inline auto OGLDrawMode(const SG::Primitive::DrawingMode& a_DrawMode)
{
    switch (a_DrawMode) {
    case SG::Primitive::DrawingMode::Points:
        return GL_POINTS;
    case SG::Primitive::DrawingMode::Lines:
        return GL_LINES;
    case SG::Primitive::DrawingMode::LineStrip:
        return GL_LINE_STRIP;
    case SG::Primitive::DrawingMode::LineLoop:
        return GL_LINE_LOOP;
    case SG::Primitive::DrawingMode::Polygon:
        return GL_POLYGON;
    case SG::Primitive::DrawingMode::Triangles:
        return GL_TRIANGLES;
    case SG::Primitive::DrawingMode::TriangleStrip:
        return GL_TRIANGLE_STRIP;
    case SG::Primitive::DrawingMode::TriangleFan:
        return GL_TRIANGLE_FAN;
    case SG::Primitive::DrawingMode::Quads:
        return GL_QUADS;
    case SG::Primitive::DrawingMode::QuadStrip:
        return GL_QUAD_STRIP;
    }
    return GL_NONE;
}

template <unsigned L, typename T, bool Normalized = false>
static inline glm::vec<L, T> ConvertData(const SG::BufferAccessor& a_Accessor, size_t a_Index)
{
    const auto componentNbr = a_Accessor.GetComponentNbr();
    glm::vec<L, T> ret {};
    for (auto i = 0u; i < L && i < componentNbr; ++i) {
        switch (a_Accessor.GetComponentType()) {
        case SG::BufferAccessor::ComponentType::Int8:
            ret[i] = a_Accessor.template GetComponent<glm::int8>(a_Index, i);
            break;
        case SG::BufferAccessor::ComponentType::Uint8:
            ret[i] = a_Accessor.template GetComponent<glm::uint8>(a_Index, i);
            break;
        case SG::BufferAccessor::ComponentType::Int16:
            ret[i] = a_Accessor.template GetComponent<glm::int16>(a_Index, i);
            break;
        case SG::BufferAccessor::ComponentType::Uint16:
            ret[i] = a_Accessor.template GetComponent<glm::uint16>(a_Index, i);
            break;
        case SG::BufferAccessor::ComponentType::Uint32:
            ret[i] = a_Accessor.template GetComponent<glm::uint32>(a_Index, i);
            break;
        case SG::BufferAccessor::ComponentType::Float16:
            ret[i] = glm::detail::toFloat32(a_Accessor.template GetComponent<glm::detail::hdata>(a_Index, i));
            break;
        case SG::BufferAccessor::ComponentType::Float32:
            ret[i] = a_Accessor.template GetComponent<glm::f32>(a_Index, i);
            break;
        default:
            throw std::runtime_error("Unknown data format");
        }
    }
    if constexpr (Normalized) {
        if constexpr (L == 4)
            return glm::vec<L, T>(glm::normalize(glm::vec3(ret)), ret.w);
        else
            return glm::normalize(ret);
    } else
        return ret;
}

inline std::vector<Vertex> ConvertVertice(const SG::Primitive& a_Primitive)
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
#ifdef _DEBUG
    assert(hasPositions);
    assert(hasNormals);
    if (hasJoints)
        assert(hasWeights);
#endif
    for (auto i = 0u; i < a_Primitive.GetPositions().GetSize(); ++i) {
        if (hasPositions)
            vertice.at(i).position = ConvertData<3, glm::f32>(a_Primitive.GetPositions(), i);
        if (hasNormals)
            vertice.at(i).normal = ConvertData<3, glm::f32, true>(a_Primitive.GetNormals(), i);
        if (hasTangent)
            vertice.at(i).tangent = ConvertData<4, glm::f32, true>(a_Primitive.GetTangent(), i);
        if (hasTexCoord_0)
            vertice.at(i).texCoord_0 = ConvertData<2, glm::f32>(a_Primitive.GetTexCoord0(), i);
        if (hasTexCoord_1)
            vertice.at(i).texCoord_1 = ConvertData<2, glm::f32>(a_Primitive.GetTexCoord1(), i);
        if (hasTexCoord_2)
            vertice.at(i).texCoord_2 = ConvertData<2, glm::f32>(a_Primitive.GetTexCoord2(), i);
        if (hasTexCoord_3)
            vertice.at(i).texCoord_3 = ConvertData<2, glm::f32>(a_Primitive.GetTexCoord3(), i);
        if (hasColor)
            vertice.at(i).color = ConvertData<3, glm::f32>(a_Primitive.GetColors(), i);
        if (hasJoints)
            vertice.at(i).joints = ConvertData<4, glm::uint16>(a_Primitive.GetJoints(), i);
        if (hasWeights)
            vertice.at(i).weights = ConvertData<4, glm::f32>(a_Primitive.GetWeights(), i);
    }
    return vertice;
}

inline std::vector<unsigned> ConvertIndice(const SG::Primitive& a_Primitive)
{
    if (a_Primitive.GetIndices().empty())
        return {};
    std::vector<unsigned> indice(a_Primitive.GetIndices().GetSize());
    auto hasIndice = !a_Primitive.GetIndices().empty();
#ifdef _DEBUG
    assert(hasIndice);
#endif
    for (auto i = 0u; i < a_Primitive.GetIndices().GetSize(); ++i) {
        indice.at(i) = ConvertData<1, glm::uint32>(a_Primitive.GetIndices(), i).x;
    }
    return indice;
}

Primitive::Primitive(RAII::Context& a_Context, SG::Primitive& a_Primitive)
    : drawMode(OGLDrawMode(a_Primitive.GetDrawingMode()))
{
    constexpr auto attribsDesc = Vertex::GetAttributeDescription();
    auto vertice               = ConvertVertice(a_Primitive);
    auto vertexBuffer          = RAII::MakePtr<RAII::Buffer>(a_Context,
        vertice.size() * sizeof(Vertex), vertice.data(), 0);

    VertexBindingDescription binding;
    binding.buffer = vertexBuffer;
    binding.index  = 0;
    binding.offset = 0;
    binding.stride = sizeof(Vertex);

    std::vector<VertexAttributeDescription> attribs(attribsDesc.begin(), attribsDesc.end());
    std::vector<VertexBindingDescription> bindings { binding };

    auto indice = ConvertIndice(a_Primitive);
    if (!indice.empty()) {
        auto indexBuffer           = RAII::MakePtr<RAII::Buffer>(a_Context,
            indice.size() * sizeof(unsigned), indice.data(), 0);
        IndexDescription indexDesc = {};
        indexDesc.type             = GL_UNSIGNED_INT;
        vertexArray                = RAII::MakePtr<RAII::VertexArray>(a_Context,
            vertice.size(), attribs, bindings,
            indice.size(), indexDesc, indexBuffer);
    } else {
        vertexArray = RAII::MakePtr<RAII::VertexArray>(a_Context,
            vertice.size(), attribs, bindings);
    }
}

void Primitive::FillGraphicsPipelineInfo(GraphicsPipelineInfo& a_PipelineInfo)
{
    a_PipelineInfo.rasterizationState.drawingMode = drawMode;
    a_PipelineInfo.vertexInputState.vertexArray   = vertexArray;
}
}
