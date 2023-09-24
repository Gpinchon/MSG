#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>

#include <GL/glew.h>
#include <algorithm>

namespace TabGraph::Renderer::RAII {
FrameBuffer::FrameBuffer()
{
    glCreateFramebuffers(1, &handle);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &handle);
}

void FrameBuffer::BindForDraw()
{
    glBindFramebuffer(
        GL_DRAW_FRAMEBUFFER,
        handle);
}

void FrameBuffer::BindForRead()
{
    glBindFramebuffer(
        GL_READ_FRAMEBUFFER,
        handle);
}

void FrameBuffer::BindNoneForDraw()
{
    glBindFramebuffer(
        GL_DRAW_FRAMEBUFFER,
        0);
}

void FrameBuffer::BindNoneForRead()
{
    glBindFramebuffer(
        GL_READ_FRAMEBUFFER,
        0);
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
    width = std::min(a_Texture.width, width);
    height = std::min(a_Texture.height, height);
}

void FrameBuffer::AttachDepthTexture(const Texture2D& a_Texture)
{
    glNamedFramebufferTexture(
        handle,
        GL_DEPTH_ATTACHMENT,
        a_Texture, 0);
    width  = std::min(a_Texture.width, width);
    height = std::min(a_Texture.height, height);
}

void FrameBuffer::AttachStencilTexture(const Texture2D& a_Texture)
{
    glNamedFramebufferTexture(
        handle,
        GL_STENCIL_ATTACHMENT,
        a_Texture, 0);
    width  = std::min(a_Texture.width, width);
    height = std::min(a_Texture.height, height);
}

void FrameBuffer::DetachColorTexture(unsigned a_ColorIndex)
{
    glNamedFramebufferTexture(
        handle,
        GL_COLOR_ATTACHMENT0 + a_ColorIndex,
        0, 0);
}
void FrameBuffer::DetachDepthexture()
{
    glNamedFramebufferTexture(
        handle,
        GL_DEPTH_ATTACHMENT,
        0, 0);
}
void FrameBuffer::DetachStencilTexture()
{
    glNamedFramebufferTexture(
        handle,
        GL_STENCIL_ATTACHMENT,
        0, 0);
}
void FrameBuffer::SetDrawBuffers(size_t a_Count, unsigned* a_Attachments)
{
    glNamedFramebufferDrawBuffers(handle, a_Count, a_Attachments);
}
void FrameBuffer::SetReadBuffer(unsigned a_Attachment)
{
    glNamedFramebufferReadBuffer(handle, a_Attachment);
}
}
