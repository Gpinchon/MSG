#include <SG/Core/Primitive.hpp>

#include <SG/Core/Buffer/Accessor.hpp>
#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/View.hpp>

namespace TabGraph::SG {
Primitive::Primitive(
    const std::vector<glm::vec3>& vertices,
    const std::vector<glm::vec3>& normals,
    const std::vector<glm::vec2>& texCoords,
    const std::vector<uint32_t> indices)
{
    const auto verticeByteSize  = vertices.size() * sizeof(glm::vec3);
    const auto normalsByteSize  = normals.size() * sizeof(glm::vec3);
    const auto texCoordByteSize = texCoords.size() * sizeof(glm::vec2);
    const auto indiceByteSize   = indices.size() * sizeof(uint32_t);
    auto vertexBuffer = std::make_shared<Buffer>(
        verticeByteSize + normalsByteSize + texCoordByteSize
    );
    std::memcpy(vertexBuffer->data(), vertices.data(), verticeByteSize);
    std::memcpy(vertexBuffer->data() + verticeByteSize, normals.data(), normalsByteSize);
    std::memcpy(vertexBuffer->data() + verticeByteSize + normalsByteSize, texCoords.data(), texCoordByteSize);
    auto indiceBuffer = std::make_shared<Buffer>(
        (std::byte*)indices.data(), indiceByteSize
    );
    const auto vertexBufferView = std::make_shared<BufferView>(vertexBuffer, 0, vertexBuffer->size());
    const auto indiceBufferView = std::make_shared<BufferView>(indiceBuffer, 0, indiceBuffer->size());
    SetPositions({
        vertexBufferView, 0,
        vertices.size(), BufferAccessor::ComponentType::Float32, 3
    });
    SetNormals({
        vertexBufferView, int(verticeByteSize),
        normals.size(), BufferAccessor::ComponentType::Float32, 3
    });
    SetTexCoord0({
        vertexBufferView, int(verticeByteSize + normalsByteSize),
        texCoords.size(), BufferAccessor::ComponentType::Float32, 2
    });
    SetIndices({
        indiceBufferView, 0,
        indices.size(), BufferAccessor::ComponentType::Uint32, 1
    });
}
Primitive::Primitive(
    const std::vector<glm::vec3>& vertices,
    const std::vector<glm::vec3>& normals,
    const std::vector<glm::vec2>& texCoords)
{
}
}