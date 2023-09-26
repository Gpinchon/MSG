#pragma once

namespace TabGraph::Renderer::RAII {
struct FrameBuffer {
    FrameBuffer(const unsigned& a_Width, const unsigned& a_Height);
    ~FrameBuffer();
    operator unsigned() const { return handle; }
    const unsigned handle = 0;
    const unsigned width = -1, height = -1;
};
}
