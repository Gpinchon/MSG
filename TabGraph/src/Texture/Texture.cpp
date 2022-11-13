/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-11 13:41:54
*/

#include <Texture/Texture.hpp>
#include <Texture/Sampler.hpp>
#include <Debug.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Texture/Texture.hpp>
#endif

#include <algorithm>
#include <array>
#include <cstring>
#include <stdint.h>
#include <utility>

using namespace TabGraph::Textures;

Texture::Texture()
    : Inherit()
{
}

Texture::Texture(const Texture& other)
    : Inherit(other)
{
}

Texture::~Texture()
{
    Unload();
}

void Texture::Load()
{
    _impl->Load();
}

void Texture::Unload()
{
    _impl->Unload();
}

void Texture::GenerateMipmap()
{
    _impl->GenerateMipmap();
}

Texture::Type Texture::GetType() const
{
    return _impl->GetType();
}

Texture::Impl& Texture::GetImpl()
{
    return *_impl;
}

void Texture::SetAutoMipMap(bool autoMipmap)
{
    return _impl->SetAutoMipMap(autoMipmap);
}

void Texture::SetSampler(std::shared_ptr<Textures::Sampler> textureSampler)
{
    return _impl->SetSampler(textureSampler);
}

bool Texture::GetLoaded() const
{
    return _impl->GetLoaded();
}

TabGraph::Pixel::Description Texture::GetPixelDescription() const
{
    return _impl->GetPixelDescription();
}

std::shared_ptr<Sampler> Texture::GetSampler() const
{
    return _impl->GetSampler();
}

void Texture::SetMipMapNbr(uint8_t mipNbr)
{
    return _impl->SetMipMapNbr(mipNbr);
}

bool Texture::GetAutoMipMap() const
{
    return _impl->GetAutoMipMap();
}

uint8_t Texture::GetMipMapNbr() const
{
    return _impl->GetMipMapNbr();
}
