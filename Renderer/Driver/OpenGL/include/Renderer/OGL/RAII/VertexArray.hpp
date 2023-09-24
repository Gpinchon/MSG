#pragma once

namespace TabGraph::Renderer::RAII {
struct VertexBuffer;
struct VertexArray {
    VertexArray(const VertexBuffer& a_VertexBuffer);
    ~VertexArray();
    operator unsigned() const
    {
        return handle;
    }
    unsigned handle;
};
}
