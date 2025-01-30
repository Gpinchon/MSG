#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

namespace MSG {
class OGLContext;
class OGLTexture2D;
}

namespace MSG {
struct OGLFrameBufferAttachment {
    unsigned attachment;
    std::shared_ptr<OGLTexture2D> texture;
};
struct OGLFrameBufferCreateInfo {
    glm::uvec3 defaultSize { -1, -1, -1 };
    std::vector<OGLFrameBufferAttachment> colorBuffers;
    std::shared_ptr<OGLTexture2D> depthBuffer;
    std::shared_ptr<OGLTexture2D> stencilBuffer;
};
class OGLFrameBuffer {
public:
    OGLFrameBuffer(OGLContext& a_Context, const OGLFrameBufferCreateInfo& a_Info);
    ~OGLFrameBuffer();
    operator unsigned() const { return handle; }
    const unsigned handle = 0;
    const OGLFrameBufferCreateInfo info;
    OGLContext& context;
};
}
