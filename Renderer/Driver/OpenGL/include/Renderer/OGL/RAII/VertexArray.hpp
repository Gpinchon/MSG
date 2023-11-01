#pragma once

#include <Renderer/OGL/IndexDescription.hpp>
#include <Renderer/OGL/VertexAttributeDescription.hpp>
#include <Renderer/OGL/VertexBindingDescription.hpp>

namespace TabGraph::Renderer::RAII {
struct Buffer;
struct VertexArray {
    VertexArray(
        const unsigned a_VertexCount,
        const std::vector<VertexAttributeDescription>& a_AttributesDesc,
        const std::vector<VertexBindingDescription>& a_VertexBindings);
    VertexArray(
        const unsigned a_VertexCount,
        const std::vector<VertexAttributeDescription>& a_AttributesDesc,
        const std::vector<VertexBindingDescription>& a_VertexBindings,
        const unsigned a_IndexCount,
        const IndexDescription& a_IndexDesc,
        const std::shared_ptr<Buffer>& a_IndexBuffer);
    ~VertexArray();

    operator unsigned() const
    {
        return handle;
    }
    const unsigned handle;
    bool indexed         = false;
    unsigned vertexCount = 0; // number of vertices
    unsigned indexCount  = 0; // number of indices (if indexed)
    std::vector<VertexAttributeDescription> attributesDesc;
    std::vector<VertexBindingDescription> vertexBindings;
    IndexDescription indexDesc;
    std::shared_ptr<Buffer> indexBuffer;
};
}
