#include <MSG/OGLContext.hpp>
#include <MSG/OGLTextureView.hpp>
#include <MSG/ToGL.hpp>

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <utility>

namespace MSG {
OGLTextureInfo GetTextureInfo(const std::shared_ptr<OGLTexture>& a_Target, const OGLTextureViewInfo& a_Info)
{
    return {
        .target      = a_Info.target,
        .width       = a_Target->width,
        .height      = a_Target->height,
        .depth       = a_Info.numLayers,
        .levels      = a_Info.numLevels,
        .sizedFormat = a_Info.sizedFormat,
    };
}

OGLTextureView::OGLTextureView(
    OGLContext& a_Context,
    const std::shared_ptr<OGLTexture>& a_Target,
    const OGLTextureViewInfo& a_Info)
    : OGLTexture(a_Context, GetTextureInfo(a_Target, a_Info), false)
{
    MSG::ExecuteOGLCommand(a_Context, [this, &target = a_Target, &info = a_Info]() mutable {
        glGenTextures(1, &handle);
        glTextureView(
            handle, info.target, *target, info.sizedFormat,
            info.minLevel, info.numLevels,
            info.minLayer, info.numLayers); }, true);
}
}