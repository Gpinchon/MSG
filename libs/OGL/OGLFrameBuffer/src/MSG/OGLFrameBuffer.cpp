#include <MSG/OGLContext.hpp>
#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLTexture2D.hpp>

#include <GL/glew.h>

namespace MSG {
auto CreateFramebuffer(OGLContext& a_Context)
{
    unsigned handle = 0;
    ExecuteOGLCommand(a_Context, [&handle] { glCreateFramebuffers(1, &handle); }, true);
    return handle;
}

OGLFrameBuffer::OGLFrameBuffer(OGLContext& a_Context, const OGLFrameBufferCreateInfo& a_Info)
    : handle(CreateFramebuffer(a_Context))
    , info(a_Info)
    , context(a_Context)
{
    ExecuteOGLCommand(context, [handle = handle, info = a_Info] {
        glNamedFramebufferParameteri(handle, GL_FRAMEBUFFER_DEFAULT_WIDTH, info.defaultSize.x);
        glNamedFramebufferParameteri(handle, GL_FRAMEBUFFER_DEFAULT_HEIGHT, info.defaultSize.y);
        glNamedFramebufferParameteri(handle, GL_FRAMEBUFFER_DEFAULT_LAYERS, info.defaultSize.z);
        for (const auto& colorBuffer : info.colorBuffers) {
            if (colorBuffer.layer > 0) [[unlikely]]
                glNamedFramebufferTextureLayer(
                    handle, colorBuffer.attachment,
                    *colorBuffer.texture, 0,
                    colorBuffer.layer);
            else
                glNamedFramebufferTexture(
                    handle, colorBuffer.attachment,
                    *colorBuffer.texture, 0);
        }
        if (info.depthBuffer.texture != nullptr) {
            if (info.depthBuffer.layer > 0) [[unlikely]]
                glNamedFramebufferTextureLayer(
                    handle, GL_DEPTH_ATTACHMENT,
                    *info.depthBuffer.texture, 0,
                    info.depthBuffer.layer);
            else
                glNamedFramebufferTexture(
                    handle, GL_DEPTH_ATTACHMENT,
                    *info.depthBuffer.texture, 0);
        }
        if (info.stencilBuffer.texture != nullptr) {
            if (info.stencilBuffer.layer > 0) [[unlikely]]
                glNamedFramebufferTextureLayer(
                    handle, GL_STENCIL_ATTACHMENT,
                    *info.stencilBuffer.texture, 0,
                    info.stencilBuffer.layer);
            else
                glNamedFramebufferTexture(
                    handle, GL_STENCIL_ATTACHMENT,
                    *info.stencilBuffer.texture, 0);
        }
        assert(glCheckNamedFramebufferStatus(handle, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    });
}

OGLFrameBuffer::~OGLFrameBuffer()
{
    ExecuteOGLCommand(context, [handle = handle] { glDeleteFramebuffers(1, &handle); });
}
}
