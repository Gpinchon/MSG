#pragma once

namespace TabGraph::Renderer::RAII {
struct FrameBuffer {
    FrameBuffer();
    ~FrameBuffer();
    operator unsigned() const { return handle; }
    unsigned handle = 0;
};
}
