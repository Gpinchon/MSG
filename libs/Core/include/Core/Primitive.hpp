#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/BoundingVolume.hpp>
#include <Core/Buffer/Accessor.hpp>
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>

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
namespace MSG::Core {
class Primitive : public Inherit<Object, Primitive> {
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
    PROPERTY(BoundingVolume, BoundingVolume, );
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
    Primitive()                       = default;
    Primitive(const Primitive& other) = default;
    inline Primitive(const std::string& a_Name)
        : Inherit(a_Name) {};
    Primitive(
        const std::vector<glm::vec3>& a_Vertices,
        const std::vector<glm::vec3>& a_Normals,
        const std::vector<glm::vec2>& a_TexCoords,
        const std::vector<uint32_t>& a_Indices);
    Primitive(
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
