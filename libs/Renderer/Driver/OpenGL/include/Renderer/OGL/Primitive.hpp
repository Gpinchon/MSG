#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

namespace MSG::Renderer::RAII {
class VertexArray;
}

namespace MSG::SG {
class Primitive;
}

namespace MSG::Renderer {
class Context;
struct GraphicsPipelineInfo;
}

namespace MSG::Renderer {
class Primitive {
public:
    Primitive(Context& a_Context, SG::Primitive& a_Primitive);
    uint32_t drawMode;
    unsigned vertexCount = 0; // number of vertices
    unsigned indexCount  = 0; // number of indices (if indexed)
    std::shared_ptr<RAII::VertexArray> vertexArray;
};
}
