/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 15:58:00
*/
#include <Texture/TextureBuffer.hpp>
#include <Texture/PixelUtils.hpp>
#include <Buffer/Accessor.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Texture/TextureBuffer.hpp>
#endif

namespace TabGraph::Textures {
TextureBuffer::TextureBuffer(Pixel::SizedFormat internalFormat, std::shared_ptr<Buffer::View> bufferAccessor)
    : Inherit()
{
    SetBuffer(bufferAccessor);
    _impl.reset(new TextureBuffer::Impl(internalFormat, bufferAccessor));
}

TextureBuffer::TextureBuffer(TextureBuffer& other)
    : Inherit(other)
{
    _impl.reset(new TextureBuffer::Impl(static_cast<const TextureBuffer::Impl&>(*other._impl.get())));
}
}
