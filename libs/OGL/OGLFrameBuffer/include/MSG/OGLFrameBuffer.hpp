#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

namespace Msg {
class OGLContext;
class OGLTexture;
}

namespace Msg {
struct OGLFrameBufferAttachment {
    unsigned attachment;
    unsigned layer = 0;
    std::shared_ptr<OGLTexture> texture;
};
struct OGLFrameBufferDepthStencilAttachment {
    unsigned layer = 0;
    std::shared_ptr<OGLTexture> texture;
};
struct OGLFrameBufferCreateInfo {
    bool layered = false; // should we use glNamedFramebufferTextureLayer
    glm::uvec3 defaultSize { 0, 0, 1 };
    std::vector<OGLFrameBufferAttachment> colorBuffers;
    OGLFrameBufferDepthStencilAttachment depthBuffer;
    OGLFrameBufferDepthStencilAttachment stencilBuffer;
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
