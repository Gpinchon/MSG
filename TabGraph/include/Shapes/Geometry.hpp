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
#include <Buffer/Accessor.hpp>
#include <Buffer/View.hpp>
#include <Core/Inherit.hpp>
#include <Shapes/Shape.hpp>

#include <array>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Renderer {
    class GeometryRenderer;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
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

class Geometry : public Core::Inherit<Shape, Geometry> {
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
    /** @brief Drawing mode for this geometry, default : GL_TRIANGLES */
    PROPERTY(DrawingMode, DrawingMode, DrawingMode::Triangles);
    PROPERTY(glm::vec3, Centroid, 0);
    PROPERTY(Buffer::Accessor, Indices, );
    PROPERTY(Buffer::Accessor, Positions, );
    PROPERTY(Buffer::Accessor, Normals, );
    PROPERTY(Buffer::Accessor, Tangent, );
    PROPERTY(Buffer::Accessor, Colors, );
    PROPERTY(Buffer::Accessor, TexCoord0, );
    PROPERTY(Buffer::Accessor, TexCoord1, );
    PROPERTY(Buffer::Accessor, TexCoord2, );
    PROPERTY(Buffer::Accessor, Joints, );
    PROPERTY(Buffer::Accessor, Weights, );

    READONLYPROPERTY(bool, Loaded, false);

public:
    Geometry();
    Geometry(const std::string& name);
    Geometry(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<uint32_t> indices, Buffer::View::Mode = Buffer::View::Mode::Immutable);
    Geometry(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, Buffer::View::Mode = Buffer::View::Mode::Immutable);
    Geometry(const Geometry& other);
    ~Geometry();
    size_t EdgeCount() const;
    glm::ivec2 GetEdge(const size_t index) const;
    size_t VertexCount() const;
};
}
