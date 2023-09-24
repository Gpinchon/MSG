#pragma once

namespace TabGraph::SG {
class Primitive;
}

namespace TabGraph::Renderer::RAII {
struct IndexBuffer {
    IndexBuffer(const SG::Primitive& a_Primitive);
    ~IndexBuffer();
    void Bind() const;
    void BindNone() const;
    operator unsigned() const
    {
        return handle;
    }
    unsigned handle = 0;
    unsigned indexFormat; //GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT
    unsigned indexCount = 0;
};
}
