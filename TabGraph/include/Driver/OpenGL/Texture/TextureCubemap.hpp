/*
* @Author: gpinchon
* @Date:   2021-05-04 15:09:46
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:25
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Driver/OpenGL/Texture/Texture.hpp>
#include <Texture/TextureCubemap.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class Declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
class TextureCubemap::Impl : public Texture::Impl {
public:
    Impl(const Impl& other);
    Impl(const glm::ivec2& size, const Pixel::SizedFormat& format);
    Impl(std::shared_ptr<Assets::Asset> image);
    ~Impl();

    void SetImage(std::shared_ptr<Assets::Asset> image);
    void SetSize(const glm::ivec2 size);
    std::shared_ptr<Assets::Asset> GetImage() const;
    glm::ivec2 GetSize() const;

    virtual void Load() override;
    virtual void Unload() override;
    virtual void GenerateMipmap() override;

private:
    void _AllocateStorage();
    void _UploadImage(std::shared_ptr<Assets::Asset> imageAsset);
    glm::ivec2 _size{ 0 };
    std::shared_ptr<Assets::Asset> _asset;
};
}
