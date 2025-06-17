#include <MSG/Image.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture2DArray.hpp>
#include <MSG/ToGL.hpp>

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <utility>

namespace MSG {
OGLTextureInfo GetTextureInfo(const OGLTexture2DArrayInfo& a_Info)
{
    return {
        .target      = GL_TEXTURE_2D_ARRAY,
        .width       = a_Info.width,
        .height      = a_Info.height,
        .depth       = a_Info.layers,
        .levels      = a_Info.levels,
        .sizedFormat = a_Info.sizedFormat,
    };
}
OGLTexture2DArray::OGLTexture2DArray(
    OGLContext& a_Context,
    const OGLTexture2DArrayInfo& a_Info)
    : OGLTexture(a_Context, GetTextureInfo(a_Info))
{
}
}