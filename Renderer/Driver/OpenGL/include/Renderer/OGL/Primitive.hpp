#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

namespace TabGraph::Renderer::RAII {
struct VertexArray;
}

namespace TabGraph::SG {
class Primitive;
}

namespace TabGraph::Renderer {
struct Context;
struct GraphicsPipelineInfo;
}

namespace TabGraph::Renderer {
struct Primitive {
    Primitive(Context& a_Context, SG::Primitive& a_Primitive);
    void FillGraphicsPipelineInfo(GraphicsPipelineInfo& a_PipelineInfo);
    uint32_t drawMode;
    unsigned vertexCount = 0; // number of vertices
    unsigned indexCount  = 0; // number of indices (if indexed)
    std::shared_ptr<RAII::VertexArray> vertexArray;
};
}
