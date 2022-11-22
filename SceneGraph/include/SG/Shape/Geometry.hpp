/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-10 01:17:40
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Buffer/Accessor.hpp>
#include <SG/Buffer/View.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Shape/Shape.hpp>

#include <array>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
/*class GeometryMorthTarget {
public:
    enum Channel {
        Normal,
        Position,
        Tangent,
        MaxChannels
    };
    inline auto Get(const GeometryMorthTarget::Channel channel) const {
        return _morphChannels.at(size_t(channel));
    }
    inline void Set(const GeometryMorthTarget::Channel channel, const std::shared_ptr<Buffer::Accessor> morphChannel) {
        _morphChannels.at(size_t(channel)) = morphChannel;
    }

private:
    std::array<std::shared_ptr<Buffer::Accessor>, GeometryMorthTarget::MaxChannels> _morphChannels;
};*/

class Geometry : public Inherit<Shape, Geometry> {
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
    /** @brief Drawing mode for this geometry, default : Triangles */
    PROPERTY(DrawingMode, DrawingMode, DrawingMode::Triangles);
    PROPERTY(glm::vec3, Centroid, 0);
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
    Geometry() = default;
    Geometry(const Geometry& other) = default;
    inline Geometry(const std::string& a_Name) : Inherit(a_Name) {};
    Geometry(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<uint32_t> indices);
    Geometry(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords);
    size_t EdgeCount() const;
    glm::ivec2 GetEdge(const size_t index) const;
    size_t VertexCount() const;
};
}
