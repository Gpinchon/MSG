/*
* @Author: gpinchon
* @Date:   2021-05-02 00:29:18
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:52
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Events/DispatchQueue.hpp>
#include <Driver/OpenGL/Texture/Texture.hpp>
#include <Events/Signal.hpp>
#include <Texture/Texture2D.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Assets {
class Asset;
}
namespace Events {
struct Event;
}
namespace Pixel {
struct Description;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
class Texture2D::Impl : public Texture::Impl {
public:
    Impl(const Impl& other);
    Impl(const glm::ivec2& size, const Pixel::Description& pixelDesc);
    Impl(std::shared_ptr<Assets::Asset>);
    ~Impl();
    virtual void Load() override;
    virtual void Unload() override;
    virtual void GenerateMipmap() override;

    void SetSize(const glm::ivec2& size);
    void SetCompressionQuality(float compression)
    {
        _compressionQuality = std::clamp(compression, 0.f, 1.f);
    }
    float GetCompressionQuality() const
    {
        return _compressionQuality;
    }
    void SetCompressed(bool);
    inline void SetImage(std::shared_ptr<Assets::Asset> asset)
    {
        _asset = asset;
    }

    inline glm::ivec2 GetSize() const
    {
        return _size;
    }
    inline bool GetCompressed() const
    {
        return _compressed;
    }
    inline std::shared_ptr<Assets::Asset> GetImage() const
    {
        return _asset;
    }

private:
    void _AllocateStorage();
    void _UploadImage();
    bool _compressed { false };
    glm::ivec2 _size { 0 };
    std::shared_ptr<Assets::Asset> _asset;
    Events::Signal<const Events::Event&>::ScoppedSlot _imageLoadingSlot;
    Events::Signal<const Events::Event&>::ScoppedSlot _imageCompressionSlot;
    Events::DispatchQueue::TaskIdentifier _imageCompressionTaskID;
    std::vector<std::byte> _imageCompressionBuffer;
    float _compressionQuality { 0.25 };
};
}