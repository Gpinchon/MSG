#include <SG/Core/Buffer/Accessor.hpp>
#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Primitive.hpp>
#include <Tools/Debug.hpp>

#include <cstring>

namespace TabGraph::SG {
Primitive::Primitive(
    const std::vector<glm::vec3>& a_Vertices,
    const std::vector<glm::vec3>& a_Normals,
    const std::vector<glm::vec2>& a_TexCoords)
{
    const auto verticeByteSize  = a_Vertices.size() * sizeof(glm::vec3);
    const auto normalsByteSize  = a_Normals.size() * sizeof(glm::vec3);
    const auto texCoordByteSize = a_TexCoords.size() * sizeof(glm::vec2);
    auto vertexBuffer           = std::make_shared<Buffer>(verticeByteSize + normalsByteSize + texCoordByteSize);
    std::memcpy(vertexBuffer->data(), a_Vertices.data(), verticeByteSize);
    std::memcpy(vertexBuffer->data() + verticeByteSize, a_Normals.data(), normalsByteSize);
    std::memcpy(vertexBuffer->data() + verticeByteSize + normalsByteSize, a_TexCoords.data(), texCoordByteSize);
    const auto vertexBufferView = std::make_shared<BufferView>(vertexBuffer, 0, vertexBuffer->size());
    SetPositions({ vertexBufferView, 0, a_Vertices.size(), DataType::Float32, 3 });
    SetNormals({ vertexBufferView, int(verticeByteSize), a_Normals.size(), DataType::Float32, 3 });
    SetTexCoord0({ vertexBufferView, int(verticeByteSize + normalsByteSize), a_TexCoords.size(), DataType::Float32, 2 });
}
Primitive::Primitive(
    const std::vector<glm::vec3>& a_Vertices,
    const std::vector<glm::vec3>& a_Normals,
    const std::vector<glm::vec2>& a_TexCoords,
    const std::vector<uint32_t>& a_Indices)
    : Primitive(a_Vertices, a_Normals, a_TexCoords)
{
    const auto indiceByteSize   = a_Indices.size() * sizeof(uint32_t);
    auto indiceBuffer           = std::make_shared<Buffer>((std::byte*)a_Indices.data(), indiceByteSize);
    const auto indiceBufferView = std::make_shared<BufferView>(indiceBuffer, 0, indiceBuffer->size());
    SetIndices({ indiceBufferView, 0, a_Indices.size(), DataType::Uint32, 1 });
}

glm::vec4 ComputeTangent(
    const glm::vec3& a_Position0,
    const glm::vec3& a_Position1,
    const glm::vec3& a_Position2,
    const glm::vec2& a_TexCoord0,
    const glm::vec2& a_TexCoord1,
    const glm::vec2& a_TexCoord2)
{
    const glm::vec3 deltaPos1 = a_Position1 - a_Position0;
    const glm::vec3 deltaPos2 = a_Position2 - a_Position0;
    const glm::vec2 deltaUV1  = a_TexCoord1 - a_TexCoord0;
    const glm::vec2 deltaUV2  = a_TexCoord2 - a_TexCoord0;
    const float deltaUV       = deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x;
    if (deltaUV == 0) {
        debugLog("Malformed mesh, texture coordinates cannot be equal between face's vertices");
        debugLog("Switching to degraded mode for this face...");
        return ComputeTangent(
            a_Position0,
            a_Position1,
            a_Position2,
            glm::vec2(0, 0),
            glm::vec2(0, 1),
            glm::vec2(1, 1));
    }
    const float r           = 1.0f / deltaUV;
    const glm::vec3 tangent = deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y;
    return { tangent, r };
}
void Primitive::GenerateTangents()
{
    if (GetDrawingMode() != DrawingMode::Triangles) {
        debugLog("Only triangulated meshes are supported for tangents generation");
        return;
    }
    if (GetPositions().empty()) {
        debugLog("Positions required for tangents calculation");
        return;
    }
    bool preciseMode = true;
    if (GetTexCoord0().empty()) {
        debugLog("TexCoord0 required for precise Tangents generation");
        debugLog("Switching to degraded mode...");
        preciseMode = false;
    }
    std::vector<glm::vec4> tangents(GetPositions().GetSize());
    auto functorPrecise = [this, &tangents = tangents, preciseMode](const uint32_t& a_I0, const uint32_t& a_I1, const uint32_t& a_I2) mutable {
        TypedBufferAccessor<glm::vec3> positions = GetPositions();
        TypedBufferAccessor<glm::vec2> texCoords = GetTexCoord0();
        auto tangent                             = ComputeTangent(
            positions.at(a_I0), positions.at(a_I1), positions.at(a_I2),
            texCoords.at(a_I0),
            texCoords.at(a_I1),
            texCoords.at(a_I2));
        tangents.at(a_I0) = tangents.at(a_I1) = tangents.at(a_I2) = tangent;
    };
    auto functor = [this, &tangents = tangents, preciseMode](const uint32_t& a_I0, const uint32_t& a_I1, const uint32_t& a_I2) mutable {
        TypedBufferAccessor<glm::vec3> positions = GetPositions();
        auto tangent                             = ComputeTangent(
            positions.at(a_I0), positions.at(a_I1), positions.at(a_I2),
            glm::vec2(0, 0),
            glm::vec2(0, 1),
            glm::vec2(1, 1));
        tangents.at(a_I0) = tangents.at(a_I1) = tangents.at(a_I2) = tangent;
    };
    if (!GetIndices().empty()) {

        for (uint32_t i = 0; i < GetIndices().GetSize(); i += 3) {
            if (GetIndices().GetComponentType() == SG::DataType::Uint32) {
                TypedBufferAccessor<uint32_t> indice = GetIndices();
                const auto& index0                   = indice.at(i + 0);
                const auto& index1                   = indice.at(i + 1);
                const auto& index2                   = indice.at(i + 2);
                preciseMode ? functorPrecise(index0, index1, index2) : functor(index0, index1, index2);
            } else if (GetIndices().GetComponentType() == SG::DataType::Uint16) {
                TypedBufferAccessor<uint16_t> indice = GetIndices();
                const auto& index0                   = indice.at(i + 0);
                const auto& index1                   = indice.at(i + 1);
                const auto& index2                   = indice.at(i + 2);
                preciseMode ? functorPrecise(index0, index1, index2) : functor(index0, index1, index2);
            }
        }
    } else {
        for (uint32_t i = 0; i < GetPositions().GetSize(); i += 3) {
            const auto index0 = i + 0;
            const auto index1 = i + 1;
            const auto index2 = i + 2;
            preciseMode ? functorPrecise(index0, index1, index2) : functor(index0, index1, index2);
        }
    }
    auto buffer           = std::make_shared<Buffer>((std::byte*)tangents.data(), tangents.size() * sizeof(glm::vec4));
    const auto bufferView = std::make_shared<BufferView>(buffer, 0, buffer->size());
    SetTangent({ bufferView, 0, tangents.size(), DataType::Float32, 4 });
}
}
