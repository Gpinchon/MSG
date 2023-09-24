#include <Renderer/OGL/Primitive.hpp>
#include <Renderer/OGL/Renderer.hpp>

#include <SG/Core/Primitive.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer {
static inline auto OGLDrawMode(const SG::Primitive::DrawingMode& a_DrawMode)
{
    switch (a_DrawMode) {
    case SG::Primitive::DrawingMode::Points:
        return GL_POINTS;
    case SG::Primitive::DrawingMode::Lines:
        return GL_LINES;
    case SG::Primitive::DrawingMode::LineStrip:
        return GL_LINE_STRIP;
    case SG::Primitive::DrawingMode::LineLoop:
        return GL_LINE_LOOP;
    case SG::Primitive::DrawingMode::Polygon:
        return GL_POLYGON;
    case SG::Primitive::DrawingMode::Triangles:
        return GL_TRIANGLES;
    case SG::Primitive::DrawingMode::TriangleStrip:
        return GL_TRIANGLE_STRIP;
    case SG::Primitive::DrawingMode::TriangleFan:
        return GL_TRIANGLE_FAN;
    case SG::Primitive::DrawingMode::Quads:
        return GL_QUADS;
    case SG::Primitive::DrawingMode::QuadStrip:
        return GL_QUAD_STRIP;
    }
    return GL_NONE;
}
Primitive::Primitive(RAII::Context& a_Context, SG::Primitive& a_Primitive)
    : drawMode(OGLDrawMode(a_Primitive.GetDrawingMode()))
    , indexBuffer(RAII::MakeWrapper<RAII::IndexBuffer>(a_Context, a_Primitive))
    , vertexBuffer(RAII::MakeWrapper<RAII::VertexBuffer>(a_Context, a_Primitive))
    , vertexArray(RAII::MakeWrapper<RAII::VertexArray>(a_Context, *vertexBuffer))
{
}

void Primitive::FillGraphicsPipelineInfo(GraphicsPipelineInfo& a_PipelineInfo)
{
    a_PipelineInfo.rasterizationState.drawingMode     = drawMode;
    a_PipelineInfo.vertexInputState.indexBuffer       = indexBuffer;
    a_PipelineInfo.vertexInputState.vertexArray       = vertexArray;
    a_PipelineInfo.vertexInputState.vertexBuffer      = vertexBuffer;
}
}
