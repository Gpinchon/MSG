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

OGLTexture::OGLTexture(OGLContext& a_Context, const unsigned& a_Target)
    : target(a_Target)
    , handle(CreateTexture(a_Context, a_Target))
    , context(a_Context)
{
}

OGLTexture::~OGLTexture()
{
    ExecuteOGLCommand(context, [handle = handle] { glDeleteTextures(1, &handle); });
}
}
