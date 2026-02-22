#include <MSG/Image.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture1D.hpp>
#include <MSG/ToGL.hpp>

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <utility>

namespace Msg {
OGLTextureInfo GetTextureInfo(const OGLTexture1DInfo& a_Info)
{
    return {
        .target      = GL_TEXTURE_1D,
        .width       = a_Info.width,
        .levels      = a_Info.levels,
        .sizedFormat = a_Info.sizedFormat,
    };
}
OGLTexture1D::OGLTexture1D(
    OGLContext& a_Context,
    const OGLTexture1DInfo& a_Info)
    : OGLTexture(a_Context, GetTextureInfo(a_Info))
{
}
}