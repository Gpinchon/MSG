/*
* @Author: gpinchon
* @Date:   2021-04-30 13:13:36
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-07 15:35:59
*/

#include <Driver/OpenGL/Texture/Texture.hpp>
#include <Texture/Sampler.hpp>

#include <GL/glew.h>
#include <stdexcept>

unsigned OpenGL::GetEnum(TabGraph::Textures::Texture::Type type)
{
    switch (type) {
    case TabGraph::Textures::Texture::Type::Texture1D:
        return GL_TEXTURE_1D;
    case TabGraph::Textures::Texture::Type::Texture1DArray:
        return GL_TEXTURE_1D_ARRAY;
    case TabGraph::Textures::Texture::Type::Texture2D:
        return GL_TEXTURE_2D;
    case TabGraph::Textures::Texture::Type::Texture2DArray:
        return GL_TEXTURE_2D_ARRAY;
    case TabGraph::Textures::Texture::Type::Texture2DMultisample:
        return GL_TEXTURE_2D_MULTISAMPLE;
    case TabGraph::Textures::Texture::Type::Texture2DMultisampleArray:
        return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
    case TabGraph::Textures::Texture::Type::Texture3D:
        return GL_TEXTURE_3D;
    case TabGraph::Textures::Texture::Type::TextureBuffer:
        return GL_TEXTURE_BUFFER;
    case TabGraph::Textures::Texture::Type::TextureCubemap:
        return GL_TEXTURE_CUBE_MAP;
    case TabGraph::Textures::Texture::Type::TextureCubemapArray:
        return GL_TEXTURE_CUBE_MAP_ARRAY;
    case TabGraph::Textures::Texture::Type::TextureRectangle:
        return GL_TEXTURE_RECTANGLE;
    default:
        throw std::runtime_error("Unknown Texture::Type");
    }
}

TabGraph::Textures::Texture::Impl::Handle OpenGL::Texture::Generate()
{
    unsigned handle;
    glGenTextures(1, &handle);
    return handle;
}

void OpenGL::Texture::Delete(TabGraph::Textures::Texture::Impl::Handle handle)
{
    glDeleteTextures(1, &handle);
}

namespace TabGraph::Textures {
Texture::Impl::Impl(const Impl& other)
    : _type(other._type)
    , _Sampler(other._Sampler)
    , _mipMapNbr(other._mipMapNbr)
    , _autoMipMap(other._autoMipMap)
    , _PixelDescription(other._PixelDescription)
{
}

Texture::Impl::Impl(Texture::Type type)
    : _type(type)
    , _Sampler(std::make_shared<Textures::Sampler>())
{
}

Texture::Impl::Impl(Texture::Type type, const Pixel::Description& description)
    : _type(type)
    , _PixelDescription(description)
    , _Sampler(std::make_shared<Textures::Sampler>())
{
}

Texture::Impl::~Impl()
{
    OpenGL::Texture::Delete(_handle);
}

void Texture::Impl::Bind() const
{
    glBindTexture(OpenGL::GetEnum(GetType()), GetHandle());
}

void Texture::Impl::Done() const
{
    glBindTexture(OpenGL::GetEnum(GetType()), 0);
}

void Texture::Impl::SetPixelDescription(const Pixel::Description& pixelDesc)
{
    if (pixelDesc == GetPixelDescription())
        return;
    _SetPixelDescription(pixelDesc);
    Unload();
}
}