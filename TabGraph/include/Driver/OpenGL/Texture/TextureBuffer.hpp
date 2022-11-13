/*
* @Author: gpinchon
* @Date:   2021-05-04 20:08:59
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:08:59
*/

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Texture/TextureBuffer.hpp>
#include <Driver/OpenGL/Texture/Texture.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Buffer {
class View;
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
class TextureBuffer::Impl : public Texture::Impl {
public:
    Impl(Pixel::SizedFormat internalFormat, std::shared_ptr<Buffer::View> bufferAccessor);
    Impl(const Impl& other);
    ~Impl();
    virtual void Load() override;
    virtual void Unload() override;
    virtual void GenerateMipmap() override;

    void SetBufferView(std::shared_ptr<Buffer::View> bufferAccessor);
    auto GetBufferView() const
    {
        return _buffer;
    }

private:
    std::shared_ptr<Buffer::View> _buffer;
};
}
