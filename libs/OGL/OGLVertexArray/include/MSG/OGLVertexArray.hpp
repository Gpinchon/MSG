#pragma once

#include <MSG/OGLIndexDescription.hpp>
#include <MSG/OGLVertexAttributeDescription.hpp>
#include <MSG/OGLVertexBindingDescription.hpp>

#include <memory>
#include <vector>

namespace Msg {
class OGLContext;
class OGLBuffer;
}

namespace Msg {
class OGLVertexArray {
public:
    OGLVertexArray(
        OGLContext& a_Context,
        const unsigned a_VertexCount,
        const std::vector<OGLVertexAttributeDescription>& a_AttributesDesc,
        const std::vector<OGLVertexBindingDescription>& a_VertexBindings);
    OGLVertexArray(
        OGLContext& a_Context,
        const unsigned a_VertexCount,
        const std::vector<OGLVertexAttributeDescription>& a_AttributesDesc,
        const std::vector<OGLVertexBindingDescription>& a_VertexBindings,
        const unsigned a_IndexCount,
        const OGLIndexDescription& a_IndexDesc,
        const std::shared_ptr<OGLBuffer>& a_IndexBuffer);
    ~OGLVertexArray();
    operator unsigned() const { return handle; }
    const unsigned handle;
    bool indexed         = false;
    unsigned vertexCount = 0; // number of vertices
    unsigned indexCount  = 0; // number of indices (if indexed)
    std::vector<OGLVertexAttributeDescription> attributesDesc;
    std::vector<OGLVertexBindingDescription> vertexBindings;
    OGLIndexDescription indexDesc;
    std::shared_ptr<OGLBuffer> indexBuffer;
    OGLContext& context;
};
}
