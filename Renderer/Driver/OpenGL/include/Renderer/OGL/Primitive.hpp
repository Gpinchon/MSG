#pragma once

#include <Renderer/OGL/RAII/IndexBuffer.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RAII/VertexBuffer.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>

namespace TabGraph::Renderer::RAII {
struct Context;
}

namespace TabGraph::SG {
struct Primitive;
}

namespace TabGraph::Renderer {
struct GraphicsPipelineInfo;
}

namespace TabGraph::Renderer {
struct Primitive {
    Primitive(RAII::Context& a_Context, SG::Primitive& a_Primitive);
    void FillGraphicsPipelineInfo(GraphicsPipelineInfo& a_PipelineInfo);
    uint32_t drawMode;
    RAII::Wrapper<RAII::IndexBuffer> indexBuffer; //if index buffer is not null, primitive is indexed
    RAII::Wrapper<RAII::VertexBuffer> vertexBuffer;
    RAII::Wrapper<RAII::VertexArray> vertexArray;
};
}
