#include <SG/Core/Primitive.hpp>

#include <SG/Core/Buffer/Accessor.hpp>
#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/View.hpp>

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

glm::vec3 ComputeTangent(
    const glm::vec3& a_Position0,
    const glm::vec3& a_Position1,
    const glm::vec3& a_Position2,
    const glm::vec2& a_TexCoord0,
    const glm::vec2& a_TexCoord1,
    const glm::vec2& a_TexCoord2)
{
    glm::vec3 normal = glm::cross((a_Position1 - a_Position0), (a_Position2 - a_Position0));
    glm::vec3 deltaPos;
    if (a_Position0 == a_Position1)
        deltaPos = a_Position2 - a_Position0;
    else
        deltaPos = a_Position1 - a_Position0;
    glm::vec2 deltaUV1 = a_TexCoord1 - a_TexCoord0;
    glm::vec2 deltaUV2 = a_TexCoord2 - a_TexCoord0;
    glm::vec3 tan;
    if (deltaUV1.s != 0)
        tan = deltaPos / deltaUV1.s;
    else
        tan = deltaPos / 1.0f;
    return glm::normalize(tan - glm::dot(normal, tan) * normal);
}
void Primitive::GenerateTangents()
{
    std::vector<glm::vec4> tangents(GetPositions().GetSize());
    if (!GetIndices().empty()) {
        for (uint i = 0; i < GetIndices().GetSize() - 3; i += 3) {
            const auto& index0    = GetIndices().at<uint32_t>(i + 0);
            const auto& index1    = GetIndices().at<uint32_t>(i + 1);
            const auto& index2    = GetIndices().at<uint32_t>(i + 2);
            const auto& position0 = GetPositions().at<glm::vec3>(index0);
            const auto& position1 = GetPositions().at<glm::vec3>(index1);
            const auto& position2 = GetPositions().at<glm::vec3>(index2);
            const auto& texCoord0 = GetTexCoord0().at<glm::vec2>(index0);
            const auto& texCoord1 = GetTexCoord0().at<glm::vec2>(index1);
            const auto& texCoord2 = GetTexCoord0().at<glm::vec2>(index2);
            auto tangent          = ComputeTangent(
                position0,
                position1,
                position2,
                texCoord0,
                texCoord1,
                texCoord2);
            tangents.at(index0) = { tangent, 1 };
            tangents.at(index1) = { tangent, 1 };
            tangents.at(index2) = { tangent, 1 };
        }
    }
    const auto indiceByteSize   = tangents.size() * sizeof(glm::vec3);
    auto indiceBuffer           = std::make_shared<Buffer>((std::byte*)tangents.data(), indiceByteSize);
    const auto indiceBufferView = std::make_shared<BufferView>(indiceBuffer, 0, indiceBuffer->size());
    SetTangent({ indiceBufferView, 0, tangents.size(), BufferAccessor::ComponentType::Float32, 3 });
}
}
