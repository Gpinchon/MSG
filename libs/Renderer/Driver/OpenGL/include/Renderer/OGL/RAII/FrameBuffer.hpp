#pragma once

#include <memory>
#include <vector>

#include <GL/glew.h>
#include <glm/vec3.hpp>

namespace MSG::Renderer::RAII {
class Texture2D;
}

namespace MSG::Renderer::RAII {
struct FrameBufferAttachment {
    GLenum attachment;
    std::shared_ptr<RAII::Texture2D> texture;
};
struct FrameBufferCreateInfo {
    glm::uvec3 defaultSize { -1, -1, -1 };
    std::vector<FrameBufferAttachment> colorBuffers;
    std::shared_ptr<RAII::Texture2D> depthBuffer;
    std::shared_ptr<RAII::Texture2D> stencilBuffer;
};
class FrameBuffer {
public:
    FrameBuffer(const FrameBufferCreateInfo& a_Info);
    ~FrameBuffer();
    operator unsigned() const { return handle; }
    const unsigned handle = 0;
    const FrameBufferCreateInfo info;
};
}
