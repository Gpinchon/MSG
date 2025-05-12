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

OGLTexture::OGLTexture(OGLContext& a_Context, const OGLTextureInfo& a_Info, const bool& a_Allocate)
    : OGLTextureInfo(a_Info)
    , handle(a_Allocate ? CreateTexture(a_Context, a_Info.target) : 0)
    , context(a_Context)
{
    if (a_Allocate)
        Allocate();
}

OGLTexture::~OGLTexture()
{
    ExecuteOGLCommand(context, [handle = handle] { glDeleteTextures(1, &handle); });
}

void OGLTexture::Allocate()
{
    switch (target) {
    case GL_TEXTURE_1D:
    case GL_PROXY_TEXTURE_1D:
        ExecuteOGLCommand(context,
            [handle = handle, info = OGLTextureInfo(*this)] {
                glTextureStorage1D(handle, info.levels, info.sizedFormat,
                    info.width);
            });
        break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_1D_ARRAY:
    case GL_TEXTURE_RECTANGLE:
    case GL_TEXTURE_CUBE_MAP:
    case GL_PROXY_TEXTURE_2D:
    case GL_PROXY_TEXTURE_1D_ARRAY:
    case GL_PROXY_TEXTURE_RECTANGLE:
    case GL_PROXY_TEXTURE_CUBE_MAP:
        ExecuteOGLCommand(context,
            [handle = handle, info = OGLTextureInfo(*this)] {
                glTextureStorage2D(handle, info.levels, info.sizedFormat,
                    info.width, info.height);
            });
        break;
    case GL_TEXTURE_3D:
    case GL_TEXTURE_2D_ARRAY:
    case GL_TEXTURE_CUBE_MAP_ARRAY:
    case GL_PROXY_TEXTURE_3D:
    case GL_PROXY_TEXTURE_2D_ARRAY:
    case GL_PROXY_TEXTURE_CUBE_MAP_ARRAY:
        ExecuteOGLCommand(context,
            [handle = handle, info = OGLTextureInfo(*this)] {
                glTextureStorage3D(handle, info.levels, info.sizedFormat,
                    info.width, info.height, info.depth);
            });
        break;
    default:
        break;
    }
}

void OGLTexture::Clear(
    const uint32_t& a_Format,
    const uint32_t& a_Type,
    const void* a_Data) const
{
    auto clearFunc = [handle = handle, format = a_Format, type = a_Type, data = a_Data] {
        glClearTexImage(
            handle,
            0, // level
            format, type, data);
    };
    ExecuteOGLCommand(context, clearFunc, true);
}
}
