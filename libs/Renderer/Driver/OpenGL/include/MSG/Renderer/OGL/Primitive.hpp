#pragma once

#include <memory>

namespace MSG {
class MeshPrimitive;
class OGLContext;
class OGLVertexArray;
}

namespace MSG::Renderer {
struct GraphicsPipelineInfo;
}

namespace MSG::Renderer {
class Primitive {
public:
    Primitive(OGLContext& a_Context, MeshPrimitive& a_Primitive);
    uint32_t drawMode;
    unsigned vertexCount = 0; // number of vertices
    unsigned indexCount  = 0; // number of indices (if indexed)
    std::shared_ptr<OGLVertexArray> vertexArray;
};
}
