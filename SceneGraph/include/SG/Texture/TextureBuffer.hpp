#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Inherit.hpp>
#include <Texture/Texture.hpp>

#include <glm/glm.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Buffer {
class View;
}

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
class TextureBuffer : public Core::Inherit<Texture, TextureBuffer> {
    class Impl;
    friend Impl;

public:
    TextureBuffer(Pixel::SizedFormat internalFormat, std::shared_ptr<Buffer::View> bufferAccessor = nullptr);
    TextureBuffer(TextureBuffer& other);
    auto& GetBuffer() const
    {
        return _buffer;
    }
    void SetBuffer(const std::shared_ptr<Buffer::View>& buffer)
    {
        _buffer = buffer;
    }

private:
    std::shared_ptr<Buffer::View> _buffer;
};
}
