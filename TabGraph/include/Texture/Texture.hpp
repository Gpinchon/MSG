/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:09:23
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>
#include <Texture/PixelUtils.hpp>

#include <map>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
class Sampler;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
class Texture : public Core::Inherit<Core::Object, Texture> {
public:
    class Impl;
    enum class Type {
        Unknown = -1,
        Texture1D,
        Texture1DArray,
        Texture2D,
        Texture2DArray,
        Texture2DMultisample,
        Texture2DMultisampleArray,
        Texture3D,
        TextureBuffer,
        TextureCubemap,
        TextureCubemapArray,
        TextureRectangle,
        MaxValue
    };
public:
    Texture(const Texture&);
    Texture();
    ~Texture();
    void Load();
    void Unload();
    void GenerateMipmap();

    Texture::Type GetType() const;
    Pixel::Description GetPixelDescription() const;
    std::shared_ptr<Sampler> GetSampler() const;
    bool GetLoaded() const;
    bool GetAutoMipMap() const;
    uint8_t GetMipMapNbr() const;

    void SetAutoMipMap(bool autoMipmap);
    void SetSampler(std::shared_ptr<Sampler> textureSampler);
    void SetMipMapNbr(uint8_t mipNbr);
    

    Impl& GetImpl();

protected:
    std::unique_ptr<Impl> _impl;
};
}
