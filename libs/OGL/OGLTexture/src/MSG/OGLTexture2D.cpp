#include <MSG/Image.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/ToGL.hpp>

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <utility>

namespace Msg {
OGLTextureInfo GetTextureInfo(const OGLTexture2DInfo& a_Info)
{
    return {
        .target      = GL_TEXTURE_2D,
        .width       = a_Info.width,
        .height      = a_Info.height,
        .levels      = a_Info.levels,
        .sizedFormat = a_Info.sizedFormat,
        .sparse      = a_Info.sparse
    };
}
OGLTexture2D::OGLTexture2D(
    OGLContext& a_Context,
    const OGLTexture2DInfo& a_Info)
    : OGLTexture(a_Context, GetTextureInfo(a_Info))
{
}
}