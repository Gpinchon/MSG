#include <MSG/Image.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture3D.hpp>
#include <MSG/ToGL.hpp>

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <utility>

namespace Msg {
OGLTextureInfo GetTextureInfo(const OGLTexture3DInfo& a_Info)
{
    return {
        .target      = GL_TEXTURE_3D,
        .width       = a_Info.width,
        .height      = a_Info.height,
        .depth       = a_Info.depth,
        .levels      = a_Info.levels,
        .sizedFormat = a_Info.sizedFormat,
    };
}
OGLTexture3D::OGLTexture3D(
    OGLContext& a_Context,
    const OGLTexture3DInfo& a_Info)
    : OGLTexture(a_Context, GetTextureInfo(a_Info))
{
}
}