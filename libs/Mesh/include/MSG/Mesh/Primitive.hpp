#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Buffer/Accessor.hpp>
#include <MSG/Core/BoundingVolume.hpp>
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Object.hpp>

#include <map>
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
class MeshPrimitive : public Core::Inherit<Core::Object, MeshPrimitive> {
public:
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
    PROPERTY(Core::BoundingVolume, BoundingVolume, );
    PROPERTY(DrawingMode, DrawingMode, DrawingMode::Triangles);
    PROPERTY(BufferAccessor, Indices, );
    PROPERTY(BufferAccessor, Positions, );
    PROPERTY(BufferAccessor, Normals, );
    PROPERTY(BufferAccessor, Tangent, );
    PROPERTY(BufferAccessor, Colors, );
    PROPERTY(BufferAccessor, TexCoord0, );
    PROPERTY(BufferAccessor, TexCoord1, );
    PROPERTY(BufferAccessor, TexCoord2, );
    PROPERTY(BufferAccessor, TexCoord3, );
    PROPERTY(BufferAccessor, Joints, );
    PROPERTY(BufferAccessor, Weights, );

public:
    MeshPrimitive()                           = default;
    MeshPrimitive(const MeshPrimitive& other) = default;
    inline MeshPrimitive(const std::string& a_Name)
        : Inherit(a_Name) {};
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
