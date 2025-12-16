#include <MSG/Image.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLTextureCubeArray.hpp>
#include <MSG/ToGL.hpp>

#include <GL/glew.h>
#include <utility>

namespace Msg {
OGLTextureInfo GetTextureInfo(const OGLTextureCubeArrayInfo& a_Info)
{
    return {
        .target      = GL_TEXTURE_CUBE_MAP_ARRAY,
        .width       = a_Info.width,
        .height      = a_Info.height,
        .depth       = a_Info.layers,
        .levels      = a_Info.levels,
        .sizedFormat = a_Info.sizedFormat,
        .sparse      = a_Info.sparse
    };
}

OGLTextureCubeArray::OGLTextureCubeArray(
    OGLContext& a_Context,
    const OGLTextureCubeArrayInfo& a_Info)
    : OGLTexture(a_Context, GetTextureInfo(a_Info))
{
}
}