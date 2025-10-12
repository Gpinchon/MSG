#include <MSG/Debug.hpp>
#include <MSG/Mesh/Primitive.hpp>

#include <functional>

namespace Msg {
MeshPrimitive::MeshPrimitive(
    const std::vector<glm::vec3>& a_Positions,
    const std::vector<glm::vec3>& a_Normals,
    const std::vector<glm::vec2>& a_TexCoords)
{
    assert(a_Positions.size() == a_Normals.size());
    assert(a_Normals.size() == a_TexCoords.size());
    std::vector<Vertex> vertice(a_Positions.size());
    for (size_t i = 0u; i < a_Positions.size(); i++) {
        vertice.at(i) = Vertex {
            .position = a_Positions.at(i),
            .normal   = a_Normals.at(i),
            .texCoord = { a_TexCoords.at(i) }
        };
    }
    SetVertices(vertice);
    ComputeBoundingVolume();
}

MeshPrimitive::MeshPrimitive(
    const std::vector<glm::vec3>& a_Vertices,
    const std::vector<glm::vec3>& a_Normals,
    const std::vector<glm::vec2>& a_TexCoords,
    const std::vector<uint32_t>& a_Indices)
    : MeshPrimitive(a_Vertices, a_Normals, a_TexCoords)
{
    SetIndices(a_Indices);
    ComputeBoundingVolume();
}

void MeshPrimitive::ComputeBoundingVolume()
{
    if (GetVerticesCount() == 0)
        return;
    auto minPos = glm::vec3 { std::numeric_limits<float>::max() };
    auto maxPos = glm::vec3 { std::numeric_limits<float>::min() };
    for (auto& vertex : GetVertices()) {
        minPos = glm::min(minPos, vertex.position);
        maxPos = glm::max(maxPos, vertex.position);
    }
    BoundingVolume BV;
    BV.SetMinMax(minPos, maxPos);
    SetBoundingVolume(BV);
}

static glm::vec4 ComputeTangent(
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

void MeshPrimitive::GenerateTangents()
{
    if (GetDrawingMode() != DrawingMode::Triangles) {
        debugLog("Only triangulated meshes are supported for tangents generation");
        return;
    }
    if (GetVerticesCount() == 0) {
        debugLog("Vertices required for tangents calculation");
        return;
    }
    bool preciseMode = true;
    if (!GetHasTexCoord()[0]) {
        debugLog("TexCoord0 required for precise Tangents generation");
        debugLog("Switching to degraded mode...");
        preciseMode = false;
    }
    auto vertices = GetVertices();
    std::vector<glm::vec4> tangents(GetVerticesCount());
    using Functor          = std::function<void(const uint32_t&, const uint32_t&, const uint32_t&)>;
    Functor functorPrecise = [this, &vertices, &tangents = tangents](const uint32_t& a_I0, const uint32_t& a_I1, const uint32_t& a_I2) mutable {
        auto tangent = ComputeTangent(
            vertices.at(a_I0).position, vertices.at(a_I1).position, vertices.at(a_I2).position,
            vertices.at(a_I0).texCoord[0], vertices.at(a_I1).texCoord[0], vertices.at(a_I2).texCoord[0]);
        tangents.at(a_I0) = tangents.at(a_I1) = tangents.at(a_I2) = tangent;
    };
    Functor functorDegraded = [this, &vertices, &tangents = tangents](const uint32_t& a_I0, const uint32_t& a_I1, const uint32_t& a_I2) mutable {
        auto tangent = ComputeTangent(
            vertices.at(a_I0).position, vertices.at(a_I1).position, vertices.at(a_I2).position,
            glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1));
        tangents.at(a_I0) = tangents.at(a_I1) = tangents.at(a_I2) = tangent;
    };
    const Functor functor = preciseMode ? functorPrecise : functorDegraded;
    if (!GetIndices().empty()) {
        for (uint32_t i = 0; i < GetIndices().size(); i += 3)
            functor(GetIndices().at(i + 0), GetIndices().at(i + 1), GetIndices().at(i + 2));
    } else {
        for (uint32_t i = 0; i < GetVerticesCount(); i += 3)
            functor(i + 0, i + 1, i + 2);
    }
    for (size_t i = 0; i < GetVerticesCount(); i++)
        vertices.at(i).tangent = tangents.at(i);
    SetVertices(vertices);
}
}

std::vector<Msg::Vertex> Msg::MeshPrimitive::GetVertices() const
{
    return GetStorage().Read();
}

void Msg::MeshPrimitive::SetVertices(const std::vector<Vertex>& a_Vector)
{
    GetStorage().Resize(a_Vector.size());
    GetStorage().Write(0, a_Vector);
}
