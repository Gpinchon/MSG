/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-12 16:30:16
*/

#include <Shapes/Geometry.hpp>
#include <Assets/Asset.hpp>
#include <Assets/BinaryData.hpp>
#include <Buffer/Accessor.hpp>
#include <Buffer/View.hpp>
#include <Cameras/Camera.hpp>
#include <Debug.hpp>
#include <Physics/BoundingAABB.hpp>
#include <Tools/Tools.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Renderer/Shapes/GeometryRenderer.hpp>
#endif

#include <algorithm>

namespace TabGraph::Shapes {
static size_t s_geometryNbr(0);
Geometry::Geometry(const std::string& name)
    : Inherit(name)
{
    _renderer.reset(new Renderer::GeometryRenderer(*this));
    ++s_geometryNbr;
}

Geometry::Geometry()
    : Geometry("Geometry_" + std::to_string(s_geometryNbr))
{
}

Geometry::Geometry(const Geometry& other)
    : Inherit(other)
{
    _renderer.reset(new Renderer::GeometryRenderer(*this));
    _Indices = other._Indices;
    _Joints = other._Joints;
    _Positions = other._Positions;
    _Normals = other._Normals;
    _TexCoord0 = other._TexCoord0;
    _TexCoord1 = other._TexCoord1;
    _TexCoord2 = other._TexCoord2;
    ++s_geometryNbr;
}

Geometry::~Geometry()
{
}

Geometry::Geometry(
    const std::vector<glm::vec3>& vertices,
    const std::vector<glm::vec3>& normals,
    const std::vector<glm::vec2>& texCoords,
    const std::vector<uint32_t> indices,
    Buffer::View::Mode mode)
    : Geometry()
{
    assert(vertices.size() == normals.size());
    assert(normals.size() == texCoords.size());
    auto vertexBuffer {
        std::make_shared<Assets::BinaryData>(
            vertices.size() * sizeof(glm::vec3) + normals.size() * sizeof(glm::vec3) + texCoords.size() * sizeof(glm::vec2) + indices.size() * sizeof(uint32_t))
    };
    auto verticeByteSize { vertices.size() * sizeof(glm::vec3) };
    auto normalsByteSize { normals.size() * sizeof(glm::vec3) };
    auto texcoordByteSize { texCoords.size() * sizeof(glm::vec2) };
    auto indiceByteSize { indices.size() * sizeof(uint32_t) };
    vertexBuffer->Set(
        (std::byte*)vertices.data(),
        0,
        verticeByteSize);
    vertexBuffer->Set(
        (std::byte*)normals.data(),
        verticeByteSize,
        normalsByteSize);
    vertexBuffer->Set(
        (std::byte*)texCoords.data(),
        verticeByteSize + normalsByteSize,
        texcoordByteSize);
    vertexBuffer->Set(
        (std::byte*)indices.data(),
        verticeByteSize + normalsByteSize + texcoordByteSize,
        indiceByteSize);
    auto vertexBufferAsset { std::make_shared<Assets::Asset>() };
    vertexBufferAsset->Get<Assets::BinaryData>().push_back(vertexBuffer);
    vertexBufferAsset->SetLoaded(true);
    auto vertexBufferView { std::make_shared<Buffer::View>(vertexBufferAsset, mode) };
    vertexBufferView->SetType(Buffer::View::Type::Array);
    vertexBufferView->SetByteLength(verticeByteSize + normalsByteSize + texcoordByteSize);

    Buffer::Accessor vertexAccessor {
        vertexBufferView,
        0, vertices.size(),
        Buffer::Accessor::ComponentType::Float32, 3
    };
    Buffer::Accessor normalAccessor {
        vertexBufferView,
        verticeByteSize, normals.size(),
        Buffer::Accessor::ComponentType::Float32, 3
    };
    Buffer::Accessor texcoordAccessor {
        vertexBufferView,
        verticeByteSize + normalsByteSize, texCoords.size(),
        Buffer::Accessor::ComponentType::Float32, 2
    };

    normalAccessor.SetNormalized(true);
    SetPositions(vertexAccessor);
    SetNormals(normalAccessor);
    SetTexCoord0(texcoordAccessor);

    if (indices.empty())
        return;
    auto indiceBufferView { std::make_shared<Buffer::View>(vertexBufferAsset, mode) };
    indiceBufferView->SetType(Buffer::View::Type::ElementArray);
    indiceBufferView->SetByteLength(indiceByteSize);
    indiceBufferView->SetByteOffset(verticeByteSize + normalsByteSize + texcoordByteSize);
    Buffer::Accessor indiceAccessor {
        indiceBufferView,
        0, indices.size(),
        Buffer::Accessor::ComponentType::Uint32, 1
    };
    SetIndices(indiceAccessor);
}

Geometry::Geometry(
    const std::vector<glm::vec3>& vertices,
    const std::vector<glm::vec3>& normals,
    const std::vector<glm::vec2>& texCoords,
    Buffer::View::Mode mode)
    : Geometry(
        vertices,
        normals,
        texCoords,
        {}, //empty indices
        mode)
{
}

size_t Geometry::EdgeCount() const
{
    switch (GetDrawingMode()) {
    case DrawingMode::Points:
        return 0;
    case DrawingMode::Lines:
        return VertexCount() / 2;
    case DrawingMode::LineStrip:
        return VertexCount() - 1;
    case DrawingMode::LineLoop:
    case DrawingMode::Polygon:
        return VertexCount();
    case DrawingMode::Triangles:
        return VertexCount() / 3;
    case DrawingMode::TriangleStrip:
    case DrawingMode::TriangleFan:
        return VertexCount() - (VertexCount() / 3) + 1;
    case DrawingMode::Quads:
        return VertexCount() / 4;
    case DrawingMode::QuadStrip:
        return VertexCount() - (VertexCount() / 4) + 1;
    }
    return 0;
}

/*
** GL_TRIANGLES
**  0 = 0 ; 1
**  1 = 1 ; 2
**  2 = 2 ; 0
**  3 = 3 ; 4
**  4 = 4 ; 5
**  5 = 5 ; 3
**  for edge B :
**   Get triangle "global" index (index / 3)
**   Multiply it by 3 to get correct offset
**   Add "local" triangle offset : (index + 1) % 3
** GL_TRIANGLE_STRIP
**  face = i / 4
**  A = (face * 2 + 1 + !((i + 2) % 4)) * (i > 0)
**  B = face * 2 + 2 * (i % 2) + !((i + 1) % 4)
**  face = (i - 1) / 2
**  A = face + 2 * !(i % 2) + !(face % 2) * (i % 2);
**  B = face + 2 * (i % 2) + (face % 2) * !(i % 2);

**  0 = 0 ; 1
**  face = 0
**  1 = 1 ; 2   a = face + 1 ; b = face + 2 //i impair  face pair
**  2 = 2 ; 0   a = face + 2 ; b = face + 0 //i pair    face pair
**  face = 1
**  3 = 1 ; 3   a = face + 0 ; b = face + 2 //i impair  face impair
**  4 = 3 ; 2   a = face + 2 ; b = face + 1 //i pair    face impair
**  face = 2
**  5 = 3 ; 4   a = face + 1 ; b = face + 2
**  6 = 4 ; 2   a = face + 2 ; b = face + 0
**  face = 3
**  7 = 3 ; 5   a = face + 0 ; b = face + 2
**  8 = 5 ; 4   a = face + 2 ; b = face + 1
**  face = 4
**  9 = 5 ; 6   a = face + 1 ; b = face + 2
** 10 = 6 ; 4   a = face + 2 ; b = face + 0

** GL_TRIANGLE_FAN

**  0 = 0 ; 1
**  face = 0
**  1 = 1 ; 2   a = face + 1 ; b = face + 2
**  2 = 2 ; 0   a = face + 2 ; b = 0
**  face = 1
**  3 = 2 ; 3   a = face + 1
**  4 = 3 ; 0   a = face + 2
**  face = 2
**  5 = 3 ; 4
**  6 = 4 ; 0

**  for edge A :
**   determin if index is odd (index % 2)
**   if index is odd, edge A equals 0
**   else edge A equals (index + 1) / 2
**   edge B equals index + 1 if index is not odd

**  GL_QUAD_STRIP
**  face = (i - 1) / 3
**  iMod3 = i % 3;
**  A = face * 2 + index % 3 + ((index + 2) % 3);
**  B = face + 2 + (3 - (index % 3)) % 3;
**      0 => 0 ; 1
**     face = 0;
**      1 => 1 ; 3  a = face * 2 + 1 + 0 ; face * 2 + 2 + 1
**      2 => 3 ; 2  a = face * 2 + 2 + 1 ; face * 2 + 1 + 1
**      3 => 2 ; 0  a = face * 2 + 0 + 2 ; face * 2 + 0 + 0
**     face = 1
**      4 => 3 ; 5  a = face * 2 + 1 + 0 ; face * 2 + 2 + 1
**      5 => 5 ; 4  a = face * 2 + 2 + 1 ; face * 2 + 1 + 1
**      6 => 4 ; 2  a = face * 2 + 0 + 2 ; face * 2 + 0 + 0
**     face = 2
**      7 => 5 ; 7  a = face * 2 + 1
**      8 => 7 ; 6  a = face * 2 + 3
**      9 => 6 ; 4  a = face * 2 + 2
*/

glm::ivec2 Geometry::GetEdge(const size_t index) const
{
    glm::ivec2 ret { -1 };
    switch (GetDrawingMode()) {
    case DrawingMode::Points:
        return glm::ivec2(index);
    case DrawingMode::Lines:
        return glm::ivec2(
            index * 2 + 0,
            index * 2 + 1);
    case DrawingMode::LineStrip:
        return glm::ivec2(
            index + 0,
            index + 1);
    case DrawingMode::LineLoop:
    case DrawingMode::Polygon:
        return glm::ivec2(
            index,
            (index + 1) % VertexCount());
    case DrawingMode::Triangles:
        return glm::ivec2(
            index,
            index / 3 * 3 + (index + 1) % 3);
    case DrawingMode::TriangleStrip: {
        auto face = (index - 1) / 2;
        auto zeroIndex = index == 0;
        auto oddIndex = index % 2;
        auto oddFace = face % 2;
        auto A = face + static_cast<unsigned long long>(2) * ~oddIndex + ~oddFace * oddIndex;
        auto B = face + 2 * oddIndex + oddFace * ~oddIndex;
        A *= ~zeroIndex;
        B += zeroIndex;
        return glm::ivec2(A, B);
    }
    case DrawingMode::TriangleFan: {
        auto face = (index - 1) / 2;
        auto zeroIndex = index == 0;
        auto oddIndex = (index % 2);
        auto A = face + 2 - oddIndex;
        auto B = (face + 2) * oddIndex;
        A *= !zeroIndex;
        B += zeroIndex;
        return glm::ivec2(A, B);
    }
    case DrawingMode::Quads:
        return glm::ivec2(
            index,
            index / 4 * 4 + (index + 1) % 4);
    case DrawingMode::QuadStrip: {
        int iMod3 = index % 3;
        int iIs3 = iMod3 == 0;
        int face = (index - 1) / 3;
        int zeroIndex = index == 0;
        int A = static_cast<unsigned long long>(face) * 2 + (iMod3) + ((index + 2) % 3);
        int B = face * 2 + 3 * ~iIs3 - iMod3 + 1 * ~iIs3;
        A *= ~zeroIndex; //perform bitwise not
        B += zeroIndex;
        return glm::ivec2(A, B);
    }
    }
    return ret;
}

size_t Geometry::VertexCount() const
{
    return GetIndices().GetSize() > 0 ? GetIndices().GetSize() : GetPositions().GetSize();
}
}