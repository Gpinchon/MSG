#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer::RAII {
FrameBuffer::FrameBuffer()
{
    glCreateFramebuffers(1, &handle);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &handle);
}

void FrameBuffer::Blit(const FrameBuffer& a_Other, const FrameBufferBlitInfo& a_Info)
{
    glBlitNamedFramebuffer(
        handle, a_Other,
        a_Info.srcX0, a_Info.srcY0, a_Info.srcX1, a_Info.srcY1,
        a_Info.dstX0, a_Info.dstY0, a_Info.dstX1, a_Info.dstY1,
        a_Info.mask, a_Info.filter);
}

void FrameBuffer::Blit(const FrameBufferBlitInfo& a_Info)
{
    glBlitNamedFramebuffer(
        handle, 0,
        a_Info.srcX0, a_Info.srcY0, a_Info.srcX1, a_Info.srcY1,
        a_Info.dstX0, a_Info.dstY0, a_Info.dstX1, a_Info.dstY1,
        a_Info.mask, a_Info.filter);
}

void FrameBuffer::AttachColorTexture(const Texture2D& a_Texture, unsigned a_ColorIndex)
{
    glNamedFramebufferTexture(
        handle,
        GL_COLOR_ATTACHMENT0 + a_ColorIndex,
        a_Texture, 0);
}

void FrameBuffer::DetachColorTexture(unsigned a_ColorIndex)
{
    glNamedFramebufferTexture(
        handle,
        GL_COLOR_ATTACHMENT0 + a_ColorIndex,
        0, 0);
}
}
