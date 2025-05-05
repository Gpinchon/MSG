#include <MSG/Debug.hpp>
#include <MSG/OGLDebugGroup.hpp>
#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLRenderPass.hpp>
#include <MSG/OGLTexture2D.hpp>

#include <GL/glew.h>
#include <iostream>

namespace MSG {
struct OGLClearFormat {
    GLenum format = GL_NONE;
    GLenum type   = GL_NONE;
};

OGLClearFormat GetClearFormat(const GLenum& a_SizedFormat);

void ApplyFBState(const OGLFrameBufferState& a_FBState, const OGLViewportState& a_Viewport)
{
    auto clearFBDebugGroup = OGLDebugGroup(__func__);
    glViewport(0, 0, a_Viewport.viewport.x, a_Viewport.viewport.y);
    glScissor(
        a_Viewport.scissorOffset.x, a_Viewport.scissorExtent.y,
        a_Viewport.scissorExtent.x, a_Viewport.scissorExtent.y);
    if (a_FBState.framebuffer == nullptr) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        return;
    }
    auto& fbInfo = a_FBState.framebuffer->info;
    for (auto& clearColor : a_FBState.clear.colors) {
        auto& colorAttachment = fbInfo.colorBuffers.at(clearColor.index);
        auto& colorBuffer     = colorAttachment.texture;
#ifndef NDEBUG
        int supported;
        glGetInternalformativ(colorBuffer->target, colorBuffer->sizedFormat, GL_CLEAR_TEXTURE, 1, &supported);
        assert(supported == GL_FULL_SUPPORT);
#endif // NDEBUG
        OGLClearFormat clearFormat { GetClearFormat(colorBuffer->sizedFormat) };
        glClearTexSubImage(
            *colorBuffer,
            0, 0, 0, colorAttachment.layer,
            a_Viewport.viewport.x, a_Viewport.viewport.y, 1,
            clearFormat.format, clearFormat.type, &clearColor.color);
    }
    if (a_FBState.clear.depthStencil.has_value()) {
        glClearTexSubImage(
            *fbInfo.depthBuffer.texture,
            0, 0, 0, fbInfo.depthBuffer.layer,
            a_Viewport.viewport.x, a_Viewport.viewport.y, 1,
            GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8_EXT, &a_FBState.clear.depthStencil.value());
    }
    if (a_FBState.clear.depth.has_value()) {
        glClearTexSubImage(
            *fbInfo.depthBuffer.texture,
            0, 0, 0, fbInfo.depthBuffer.layer,
            a_Viewport.viewport.x, a_Viewport.viewport.y, 1,
            GL_DEPTH_COMPONENT, GL_FLOAT, &a_FBState.clear.depth.value());
    }
    if (a_FBState.clear.stencil.has_value()) {
        glClearTexSubImage(
            *fbInfo.depthBuffer.texture,
            0, 0, 0, fbInfo.depthBuffer.layer,
            a_Viewport.viewport.x, a_Viewport.viewport.y, 1,
            GL_STENCIL_INDEX, GL_INT, &a_FBState.clear.stencil.value());
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *a_FBState.framebuffer);
    if (!a_FBState.drawBuffers.empty())
        glNamedFramebufferDrawBuffers(
            *a_FBState.framebuffer,
            GLsizei(a_FBState.drawBuffers.size()), a_FBState.drawBuffers.data());
    else
        glDrawBuffer(GL_NONE);
}

OGLRenderPass::OGLRenderPass(const OGLRenderPassInfo& a_Info)
    : info(a_Info)
{
}

void OGLRenderPass::Begin() const
{
    auto debugGroup = OGLDebugGroup("Execute Pass : " + info.name);
    ApplyFBState(info.frameBufferState, info.viewportState);
}

void OGLRenderPass::End() const
{
    ApplyFBState({}, {});
}

OGLClearFormat GetClearFormat(const GLenum& a_SizedFormat)
{
    OGLClearFormat format;
    switch (a_SizedFormat) {
    case GL_R8:
        format.format = GL_RED;
        format.type   = GL_FLOAT;
        break;
    case GL_R8_SNORM:
        format.format = GL_RED;
        format.type   = GL_FLOAT;
        break;
    case GL_R8UI:
        format.format = GL_RED_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_R8I:
        format.format = GL_RED_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_R16:
        format.format = GL_RED;
        format.type   = GL_FLOAT;
        break;
    case GL_R16_SNORM:
        format.format = GL_RED;
        format.type   = GL_FLOAT;
        break;
    case GL_R16UI:
        format.format = GL_RED_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_R16I:
        format.format = GL_RED_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_R32UI:
        format.format = GL_RED_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_R32I:
        format.format = GL_RED_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_R16F:
        format.format = GL_RED;
        format.type   = GL_FLOAT;
        break;
    case GL_R32F:
        format.format = GL_RED;
        format.type   = GL_FLOAT;
        break;
    case GL_RG8:
        format.format = GL_RG;
        format.type   = GL_UNSIGNED_BYTE;
        break;
    case GL_RG8_SNORM:
        format.format = GL_RG;
        format.type   = GL_FLOAT;
        break;
    case GL_RG8UI:
        format.format = GL_RG_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RG8I:
        format.format = GL_RG_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RG16:
        format.format = GL_RG;
        format.type   = GL_FLOAT;
        break;
    case GL_RG16_SNORM:
        format.format = GL_RG;
        format.type   = GL_FLOAT;
        break;
    case GL_RG16UI:
        format.format = GL_RG_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RG16I:
        format.format = GL_RG_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RG32UI:
        format.format = GL_RG_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RG32I:
        format.format = GL_RG_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RG16F:
        format.format = GL_RG;
        format.type   = GL_FLOAT;
        break;
    case GL_RG32F:
        format.format = GL_RG;
        format.type   = GL_FLOAT;
        break;
    case GL_RGB8:
        format.format = GL_RGB;
        format.type   = GL_FLOAT;
        break;
    case GL_RGB8_SNORM:
        format.format = GL_RGB;
        format.type   = GL_FLOAT;
        break;
    case GL_RGB8UI:
        format.format = GL_RGB_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RGB8I:
        format.format = GL_RGB_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RGB16:
        format.format = GL_RGB;
        format.type   = GL_FLOAT;
        break;
    case GL_RGB16_SNORM:
        format.format = GL_RGB;
        format.type   = GL_FLOAT;
        break;
    case GL_RGB16UI:
        format.format = GL_RGB_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RGB16I:
        format.format = GL_RGB_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RGB32UI:
        format.format = GL_RGB_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RGB32I:
        format.format = GL_RGB_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RGB16F:
        format.format = GL_RGB;
        format.type   = GL_FLOAT;
        break;
    case GL_RGB32F:
        format.format = GL_RGB;
        format.type   = GL_FLOAT;
        break;
    case GL_RGBA8:
        format.format = GL_RGBA;
        format.type   = GL_FLOAT;
        break;
    case GL_RGBA8_SNORM:
        format.format = GL_RGBA;
        format.type   = GL_FLOAT;
        break;
    case GL_RGBA8UI:
        format.format = GL_RGBA_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RGBA8I:
        format.format = GL_RGBA_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RGBA16:
        format.format = GL_RGBA;
        format.type   = GL_FLOAT;
        break;
    case GL_RGBA16_SNORM:
        format.format = GL_RGBA;
        format.type   = GL_FLOAT;
        break;
    case GL_RGBA16UI:
        format.format = GL_RGBA_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RGBA16I:
        format.format = GL_RGBA_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RGBA32UI:
        format.format = GL_RGBA_INTEGER;
        format.type   = GL_UNSIGNED_INT;
        break;
    case GL_RGBA32I:
        format.format = GL_RGBA_INTEGER;
        format.type   = GL_INT;
        break;
    case GL_RGBA16F:
        format.format = GL_RGBA;
        format.type   = GL_FLOAT;
        break;
    case GL_RGBA32F:
        format.format = GL_RGBA;
        format.type   = GL_FLOAT;
        break;
    case GL_RGBA_DXT5_S3TC:
        format.format = GL_RGBA;
        format.type   = GL_RGBA_DXT5_S3TC;
        break;
    default:
        errorFatal("Unknown Format");
    }
    return format;
}
}
