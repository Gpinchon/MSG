#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/BoundingVolume.hpp>
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Object.hpp>
#include <MSG/Core/Property.hpp>
#include <MSG/Mesh/PrimitiveStorage.hpp>
#include <MSG/Mesh/Vertex.hpp>

#include <array>
#include <vector>

#include <glm/glm.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
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
    PROPERTY(PrimitiveStorage, Storage, );
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
    size_t GetVerticesCount() const { return GetStorage().GetSize(); };
    std::vector<Vertex> GetVertices() const;
    void SetVertices(const std::vector<Vertex>& a_Vector);
};
}
