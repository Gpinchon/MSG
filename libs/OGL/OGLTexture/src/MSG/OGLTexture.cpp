#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture.hpp>

#include <GL/glew.h>

namespace MSG {
static inline auto CreateTexture(OGLContext& a_Context, const GLenum& a_Target)
{
    GLuint handle = 0;
    ExecuteOGLCommand(a_Context, [&handle, &a_Target] { glCreateTextures(a_Target, 1, &handle); }, true);
    return handle;
}

OGLTexture::OGLTexture(OGLContext& a_Context,
    const unsigned& a_Target, const unsigned& a_SizedFormat,
    const unsigned& a_Width, const unsigned& a_Height, const unsigned& a_Depth,
    const unsigned& a_Levels)
    : handle(CreateTexture(a_Context, a_Target))
    , target(a_Target)
    , sizedFormat(a_SizedFormat)
    , width(a_Width)
    , height(a_Height)
    , depth(a_Depth)
    , levels(a_Levels)
    , context(a_Context)
{
    // ExecuteOGLCommand(context, [handle = handle, levels = levels, sizedFormat = sizedFormat, width = width, height = height, depth = depth] {
    //     glTextureStorage3D(handle, levels, sizedFormat, width, height, depth);
    // });
}

OGLTexture::~OGLTexture()
{
    ExecuteOGLCommand(context, [handle = handle] { glDeleteTextures(1, &handle); });
}
}
