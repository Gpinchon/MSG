#include <Renderer/OGL/RAII/Texture.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer::RAII {
Texture2D::Texture2D(unsigned a_Width, unsigned a_Height, unsigned a_Levels, unsigned a_Format)
    : width(a_Width)
    , height(a_Height)
    , levels(a_Levels)
    , format(a_Format)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &handle);
    glTextureStorage2D(handle, a_Levels, a_Format, a_Width, a_Height);
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &handle);
}
}
