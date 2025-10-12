#include <MSG/Image.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLTextureCube.hpp>
#include <MSG/ToGL.hpp>

#include <GL/glew.h>
#include <utility>

namespace Msg {
OGLTextureInfo GetTextureInfo(const OGLTextureCubeInfo& a_Info)
{
    return {
        .target      = GL_TEXTURE_CUBE_MAP,
        .width       = a_Info.width,
        .height      = a_Info.height,
        .depth       = 6,
        .levels      = a_Info.levels,
        .sizedFormat = a_Info.sizedFormat,
        .sparse      = a_Info.sparse
    };
}

OGLTextureCube::OGLTextureCube(
    OGLContext& a_Context,
    const OGLTextureCubeInfo& a_Info)
    : OGLTexture(a_Context, GetTextureInfo(a_Info))
{
}
}