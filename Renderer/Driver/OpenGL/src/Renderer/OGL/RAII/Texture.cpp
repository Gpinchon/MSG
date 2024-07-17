#include <Renderer/OGL/RAII/Texture.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer::RAII {
static inline auto CreateTexture(const GLenum& a_Target)
{
    GLuint handle = 0;
    glCreateTextures(a_Target, 1, &handle);
    return handle;
}

Texture2D::Texture2D(
    const unsigned& a_Width,
    const unsigned& a_Height,
    const unsigned& a_Levels,
    const unsigned& a_SizedFormat)
    : Texture(GL_TEXTURE_2D)
    , width(a_Width)
    , height(a_Height)
    , levels(a_Levels)
    , sizedFormat(a_SizedFormat)
{
    glTextureStorage2D(handle, a_Levels, sizedFormat, a_Width, a_Height);
}

Texture::Texture(const unsigned& a_Target)
    : target(a_Target)
    , handle(CreateTexture(a_Target))
{
}

Texture::~Texture()
{
    glDeleteTextures(1, &handle);
}
}
