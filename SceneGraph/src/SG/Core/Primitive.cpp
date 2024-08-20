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
    SetPositions({ vertexBufferView, 0, a_Vertices.size(), BufferAccessor::ComponentType::Float32, 3 });
    SetNormals({ vertexBufferView, int(verticeByteSize), a_Normals.size(), BufferAccessor::ComponentType::Float32, 3 });
    SetTexCoord0({ vertexBufferView, int(verticeByteSize + normalsByteSize), a_TexCoords.size(), BufferAccessor::ComponentType::Float32, 2 });
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
    SetIndices({ indiceBufferView, 0, a_Indices.size(), BufferAccessor::ComponentType::Uint32, 1 });
}

glm::vec4 ComputeTangent(
    const glm::vec3& a_Position0,
    const glm::vec3& a_Position1,
    const glm::vec3& a_Position2,
    const glm::vec2& a_TexCoord0,
    const glm::vec2& a_TexCoord1,
    const glm::vec2& a_TexCoord2)
{
    glm::vec3 deltaPos1 = a_Position1 - a_Position0;
    glm::vec3 deltaPos2 = a_Position2 - a_Position0;
    glm::vec2 deltaUV1  = a_TexCoord1 - a_TexCoord0;
    glm::vec2 deltaUV2  = a_TexCoord2 - a_TexCoord0;
    float r             = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    glm::vec3 tangent   = deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y;
    return { tangent, r };
}
void Primitive::GenerateTangents()
{
    if (GetDrawingMode() != DrawingMode::Triangles)
        throw std::runtime_error("Only triangulated meshes are supported for tangents generation");
    if (GetPositions().empty())
        throw std::runtime_error("Positions required for tangents calculation");
    bool preciseMode = true;
    if (GetTexCoord0().empty()) {
        debugLog("TexCoord0 required for precise Tangents generation");
        debugLog("Switching to degraded mode...");
        preciseMode = false;
    }
    std::vector<glm::vec4> tangents(GetPositions().GetSize());
    auto functor = [this, &tangents = tangents, preciseMode](const uint32_t& a_I0, const uint32_t& a_I1, const uint32_t& a_I2) mutable {
        auto tangent = ComputeTangent(
            GetPositions().at<glm::vec3>(a_I0),
            GetPositions().at<glm::vec3>(a_I1),
            GetPositions().at<glm::vec3>(a_I2),
            preciseMode ? GetTexCoord0().at<glm::vec2>(a_I0) : glm::vec2(0, 0),
            preciseMode ? GetTexCoord0().at<glm::vec2>(a_I1) : glm::vec2(0, 1),
            preciseMode ? GetTexCoord0().at<glm::vec2>(a_I2) : glm::vec2(1, 1));
        tangents.at(a_I0) = tangents.at(a_I1) = tangents.at(a_I2) = tangent;
    };
    if (!GetIndices().empty()) {
        for (uint32_t i = 0; i < GetIndices().GetSize(); i += 3) {
            if (GetIndices().GetComponentType() == SG::BufferAccessor::ComponentType::Uint32) {
                const auto& index0 = GetIndices().at<uint32_t>(i + 0);
                const auto& index1 = GetIndices().at<uint32_t>(i + 1);
                const auto& index2 = GetIndices().at<uint32_t>(i + 2);
                functor(index0, index1, index2);
            } else if (GetIndices().GetComponentType() == SG::BufferAccessor::ComponentType::Uint16) {
                const auto& index0 = GetIndices().at<uint16_t>(i + 0);
                const auto& index1 = GetIndices().at<uint16_t>(i + 1);
                const auto& index2 = GetIndices().at<uint16_t>(i + 2);
                functor(index0, index1, index2);
            }
        }
    } else {
        for (uint32_t i = 0; i < GetPositions().GetSize(); i += 3) {
            functor(i, i + 1, i + 2);
        }
    }
    auto buffer           = std::make_shared<Buffer>((std::byte*)tangents.data(), tangents.size() * sizeof(glm::vec4));
    const auto bufferView = std::make_shared<BufferView>(buffer, 0, buffer->size());
    SetTangent({ bufferView, 0, tangents.size(), BufferAccessor::ComponentType::Float32, 4 });
}
}
