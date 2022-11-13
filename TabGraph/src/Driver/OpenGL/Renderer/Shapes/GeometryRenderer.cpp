/*
* @Author: gpinchon
* @Date:   2021-03-22 20:52:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-04-12 18:39:32
*/

#include <Driver/OpenGL/Renderer/Shapes/GeometryRenderer.hpp>
#include <Buffer/Accessor.hpp>
#include <Driver/OpenGL/Buffer.hpp>
#include <Driver/OpenGL/VertexArray.hpp>
#include <Shapes/Geometry.hpp>

#include <GL/glew.h>
#include <Tools/Tools.hpp>

using namespace TabGraph;

auto GLDrawingMode(Shapes::Geometry::DrawingMode drawingMode)
{
    static std::array<GLenum, (size_t)Shapes::Geometry::DrawingMode::MaxValue> s_drawingModeLUT = {
        GL_POINTS, //Points
        GL_LINES, //Lines
        GL_LINE_STRIP, //LineStrip
        GL_LINE_LOOP, //LineLoop
        GL_POLYGON, //Polygon
        GL_TRIANGLES, //Triangles
        GL_TRIANGLE_STRIP, //TriangleStrip
        GL_TRIANGLE_FAN, //TriangleFan
        GL_QUADS, //Quads
        GL_QUAD_STRIP //QuadStrip
    };
    return s_drawingModeLUT.at(int(drawingMode));
}

static inline auto GLComponentType(Buffer::Accessor::ComponentType type)
{
    static std::array<GLenum, (size_t)Buffer::Accessor::ComponentType::MaxValue> s_compTypeLUT = {
        GL_BYTE, //Int8
        GL_UNSIGNED_BYTE, //Uint8
        GL_SHORT, //Int16
        GL_UNSIGNED_SHORT, //Uint16
        GL_UNSIGNED_INT, //Uint32
        GL_FLOAT //Float32
    };
    return s_compTypeLUT.at(int(type));
}

namespace TabGraph::Renderer {
GeometryRenderer::GeometryRenderer(Shapes::Geometry& geometry)
    : _geometry(geometry)
{
}

GeometryRenderer::~GeometryRenderer()
{
}

void GeometryRenderer::OnFrameBegin(const Renderer::Options&)
{
}

void GeometryRenderer::Render(bool doubleSided)
{
    if (_vao == nullptr) {
        _vao = std::make_unique<OpenGL::VertexArray>();
        _vao->Bind()
            .BindAccessor(_geometry.GetPositions(), 0)
            .BindAccessor(_geometry.GetNormals(), 1)
            .BindAccessor(_geometry.GetTangent(), 2)
            .BindAccessor(_geometry.GetTexCoord0(), 3)
            .BindAccessor(_geometry.GetTexCoord1(), 4)
            .BindAccessor(_geometry.GetColors(), 5)
            .BindAccessor(_geometry.GetJoints(), 6)
            .BindAccessor(_geometry.GetWeights(), 7)
            .Done();
        if (_geometry.GetIndices().GetSize() != 0)
            _geometry.GetIndices().GetBufferView()->Load();
    }
    if (doubleSided)
        glDisable(GL_CULL_FACE);
    else
        glEnable(GL_CULL_FACE);
    _vao->Bind();
    if (_geometry.GetIndices().GetSize() != 0) {
        auto drawingMode { GLDrawingMode(_geometry.GetDrawingMode()) };
        auto compType { GLComponentType(_geometry.GetIndices().GetComponentType()) };
        auto bufferView(_geometry.GetIndices().GetBufferView());
        auto byteOffset(_geometry.GetIndices().GetByteOffset());
        OpenGL::Bind(bufferView);
        glDrawElements(drawingMode, _geometry.GetIndices().GetSize(), compType, BUFFER_OFFSET(byteOffset));
        OpenGL::Bind(nullptr, bufferView->GetType());
    } else if (auto accessor(_geometry.GetPositions()); accessor.GetSize() > 0) {
        auto drawingMode { GLDrawingMode(_geometry.GetDrawingMode()) };
        glDrawArrays(drawingMode, 0, accessor.GetSize());
    }
    OpenGL::VertexArray::BindNone();
}

void GeometryRenderer::OnFrameEnd(const Renderer::Options&)
{
}

void GeometryRenderer::Render(const::Renderer::Options& options, const glm::mat4& parentTransform, const glm::mat4& parentLastTransform)
{
    Render();
}
};
