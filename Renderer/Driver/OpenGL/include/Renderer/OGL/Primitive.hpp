#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

namespace TabGraph::Renderer::RAII {
class VertexArray;
}

namespace TabGraph::SG {
class Primitive;
}

namespace TabGraph::Renderer {
class Context;
struct GraphicsPipelineInfo;
}

namespace TabGraph::Renderer {
class Primitive {
public:
    Primitive(Context& a_Context, SG::Primitive& a_Primitive);
    uint32_t drawMode;
    unsigned vertexCount = 0; // number of vertices
    unsigned indexCount  = 0; // number of indices (if indexed)
    std::shared_ptr<RAII::VertexArray> vertexArray;
};
}
