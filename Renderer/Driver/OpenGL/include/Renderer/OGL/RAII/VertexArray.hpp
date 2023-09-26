#pragma once

namespace TabGraph::Renderer::RAII {
struct VertexBuffer;
struct IndexBuffer;
struct VertexArray {
    VertexArray(const VertexBuffer& a_VertexBuffer);
    VertexArray(const VertexBuffer& a_VertexBuffer, const IndexBuffer& a_IndexBuffer);
    ~VertexArray();
    operator unsigned() const
    {
        return handle;
    }
    const unsigned handle;
};
}
