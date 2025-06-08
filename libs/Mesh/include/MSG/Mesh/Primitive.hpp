#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/BoundingVolume.hpp>
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Object.hpp>
#include <MSG/Core/Property.hpp>

#include <array>
#include <string>
#include <vector>

#include <glm/glm.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
struct Vertex {
    static constexpr auto TexCoordCount = 4;

    glm::vec3 position = { 0, 0, 0 };
    glm::vec3 normal   = { 1, 0, 0 };
    glm::vec4 tangent  = { 0, 1, 0, 0 };
    glm::vec2 texCoord[TexCoordCount];
    glm::vec3 color   = { 1, 1, 1 };
    glm::vec4 joints  = { 0, 0, 0, 0 };
    glm::vec4 weights = { 1, 0, 0, 0 };
};

class MeshPrimitive : public Core::Inherit<Core::Object, MeshPrimitive> {
public:
    typedef std::array<bool, Vertex::TexCoordCount> TexCoordBools;
    enum class DrawingMode {
        Unknown = -1,
        Points,
        Lines,
        LineStrip,
        LineLoop,
        Polygon,
        Triangles,
        TriangleStrip,
        TriangleFan,
        Quads,
        QuadStrip,
        MaxValue
    };
    PROPERTY(bool, CastShadow, false);
    PROPERTY(BoundingVolume, BoundingVolume, );
    PROPERTY(DrawingMode, DrawingMode, DrawingMode::Triangles);
    PROPERTY(TexCoordBools, HasTexCoord, );
    PROPERTY(std::vector<Vertex>, Vertices, );
    PROPERTY(std::vector<uint32_t>, Indices, );

public:
    MeshPrimitive()                           = default;
    MeshPrimitive(const MeshPrimitive& other) = default;
    inline MeshPrimitive(const std::string& a_Name)
        : Inherit(a_Name) { };
    MeshPrimitive(
        const std::vector<glm::vec3>& a_Vertices,
        const std::vector<glm::vec3>& a_Normals,
        const std::vector<glm::vec2>& a_TexCoords,
        const std::vector<uint32_t>& a_Indices);
    MeshPrimitive(
        const std::vector<glm::vec3>& a_Vertices,
        const std::vector<glm::vec3>& a_Normals,
        const std::vector<glm::vec2>& a_TexCoords);
    void ComputeBoundingVolume();
    /**
     * @brief Generate the tangents from vertices, tangents might be required by renderers
     */
    void GenerateTangents();
};
}
