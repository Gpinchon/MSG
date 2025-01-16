#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>

#include <GL/glew.h>
#include <algorithm>

namespace TabGraph::Renderer::RAII {
auto CreateFramebuffer()
{
    unsigned handle = 0;
    glCreateFramebuffers(1, &handle);
    return handle;
}

FrameBuffer::FrameBuffer(const FrameBufferCreateInfo& a_Info)
    : handle(CreateFramebuffer())
    , info(a_Info)
{
    glNamedFramebufferParameteri(handle, GL_FRAMEBUFFER_DEFAULT_WIDTH, info.defaultSize.x);
    glNamedFramebufferParameteri(handle, GL_FRAMEBUFFER_DEFAULT_HEIGHT, info.defaultSize.y);
    glNamedFramebufferParameteri(handle, GL_FRAMEBUFFER_DEFAULT_LAYERS, info.defaultSize.z);
    for (const auto& colorBuffer : info.colorBuffers) {
        glNamedFramebufferTexture(
            handle, colorBuffer.attachment,
            *colorBuffer.texture, 0);
    }
    if (info.depthBuffer != nullptr) {
        glNamedFramebufferTexture(
            handle, GL_DEPTH_ATTACHMENT,
            *info.depthBuffer, 0);
    }
    if (info.stencilBuffer != nullptr) {
        glNamedFramebufferTexture(
            handle, GL_STENCIL_ATTACHMENT,
            *info.stencilBuffer, 0);
    }
    assert(glCheckNamedFramebufferStatus(handle, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &handle);
}
}
