/*
* @Author: gpinchon
* @Date:   2021-05-04 20:12:42
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:12:42
*/

#include "Driver/OpenGL/Texture/TextureBuffer.hpp"
#include "Driver/OpenGL/Texture/PixelUtils.hpp"
#include "Driver/OpenGL/Buffer.hpp"
#include "Buffer/Accessor.hpp"

#include <GL/glew.h>

namespace TabGraph::Textures {
TextureBuffer::Impl::Impl(Pixel::SizedFormat internalFormat, std::shared_ptr<Buffer::View> bufferAccessor)
	: Texture::Impl(Texture::Type::TextureBuffer, internalFormat)
    , _buffer(bufferAccessor)
{
}

TextureBuffer::Impl::Impl(const Impl& other)
    : Texture::Impl(other)
    , _buffer(other._buffer)
{
    _type = Texture::Type::TextureBuffer;
}

TextureBuffer::Impl::~Impl()
{
}

void TextureBuffer::Impl::Load()
{
	if (GetLoaded())
		return;
    _handle = OpenGL::Texture::Generate();
    Bind();
    glTexBuffer(
        OpenGL::GetEnum(Texture::Type::TextureBuffer),
        OpenGL::GetEnum(GetPixelDescription().GetSizedFormat()),
        OpenGL::GetHandle(GetBufferView())
    );
    Done();
    SetLoaded(true);
}

void TextureBuffer::Impl::Unload()
{
	if (!GetLoaded())
		return;
	OpenGL::Texture::Delete(GetHandle());
    SetLoaded(false);
}

void TextureBuffer::Impl::GenerateMipmap()
{
}

void TextureBuffer::Impl::SetBufferView(std::shared_ptr<Buffer::View> bufferAccessor)
{
    if (bufferAccessor == GetBufferView())
        return;
    _buffer = bufferAccessor;
    Unload();
}

}
