/*
* @Author: gpinchon
* @Date:   2021-01-08 17:02:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:41:34
*/

#include <Renderer/Framebuffer.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Texture/Framebuffer.hpp>
#endif

namespace TabGraph::Renderer {
Framebuffer::Framebuffer(glm::ivec2 size)
    : _impl(new Framebuffer::Impl(*this))
{
    SetSize(size);
}

Framebuffer::~Framebuffer()
{    
}

glm::ivec2 Framebuffer::GetSize() const
{
    return _impl->GetSize();
}

void Framebuffer::SetSize(const glm::ivec2& new_size)
{
    _impl->SetSize(new_size);
}

void Framebuffer::AddColorBuffer(std::shared_ptr<Textures::Texture> attachement, unsigned mipLevel)
{
    return _impl->AddColorBuffer(attachement, mipLevel);
}

void Framebuffer::SetColorBuffer(std::shared_ptr<Textures::Texture> buffer, unsigned index, unsigned mipLevel)
{
    return _impl->SetColorBuffer(buffer, index, mipLevel);
}

void Framebuffer::SetStencilBuffer(std::shared_ptr<Textures::Texture> buffer, unsigned mipLevel)
{
    return _impl->SetStencilBuffer(buffer, mipLevel);
}

void Framebuffer::SetDepthBuffer(std::shared_ptr<Textures::Texture> buffer, unsigned mipLevel)
{
    return _impl->SetDepthBuffer(buffer, mipLevel);
}

void Framebuffer::BlitTo(std::shared_ptr<Framebuffer> to, glm::ivec2 src0, glm::ivec2 src1, glm::ivec2 dst0, glm::ivec2 dst1, BufferMask mask, Textures::Sampler::Filter filter)
{
    return _impl->BlitTo(to, src0, src1, dst0, dst1, mask, filter);
}

void Framebuffer::BlitTo(std::shared_ptr<Framebuffer> to, BufferMask mask, Textures::Sampler::Filter filter)
{
    return _impl->BlitTo(to, mask, filter);
}

void Framebuffer::BlitTo(std::shared_ptr<Core::Window> to, BufferMask mask, Textures::Sampler::Filter filter)
{
    return _impl->BlitTo(to, mask, filter);
}

std::shared_ptr<Textures::Texture> Framebuffer::GetColorBuffer(unsigned index)
{
    return _impl->GetColorBuffer(index);
}

std::shared_ptr<Textures::Texture> Framebuffer::GetDepthBuffer()
{
    return _impl->GetDepthBuffer();
}

std::shared_ptr<Textures::Texture> Framebuffer::GetStencilBuffer()
{
    return _impl->GetStencilBuffer();
}

Framebuffer::Impl& Framebuffer::GetImpl()
{
    return *_impl.get();
}

}
