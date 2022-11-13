/*
* @Author: gpinchon
* @Date:   2021-05-11 00:56:26
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-11 13:49:32
*/

#include <Driver/OpenGL/Texture/Framebuffer.hpp>
#include <Driver/OpenGL/Texture/Texture.hpp>
#include <Driver/OpenGL/Texture/Sampler.hpp>
#include <Shader/Global.hpp>
#include <Texture/Texture2D.hpp>
#include <Texture/TextureCubemap.hpp>
#include <Window.hpp>

#include <stdexcept>
#include <GL/glew.h>

namespace TabGraph::Renderer {
auto ResizeAttachement(std::shared_ptr<Textures::Texture> attachement, glm::ivec2 res)
{
    switch (attachement->GetType())
    {
    case (Textures::Texture::Type::Texture2D):
        std::static_pointer_cast<Textures::Texture2D>(attachement)->SetSize(res);
        break;
    case (Textures::Texture::Type::TextureCubemap):
        std::static_pointer_cast<Textures::TextureCubemap>(attachement)->SetSize(res);
        break;
    }
}

Framebuffer::Impl::Impl(const Framebuffer& framebuffer)
    : _framebuffer(framebuffer)
{
    glGenFramebuffers(1, &_handle);
}

Framebuffer::Impl::~Impl()
{
    glDeleteFramebuffers(1, &_handle);
}

Framebuffer::Impl::Handle Framebuffer::Impl::GetHandle()
{
    return _handle;
}

glm::ivec2 Framebuffer::Impl::GetSize() const
{
    return _size;
}

void Framebuffer::Impl::SetSize(const glm::ivec2 size)
{
    if (GetSize() == size) {
        return;
    }
    _size = size;
    for (auto& attachement : _colorBuffers) {
        if (attachement.first != nullptr)
            ResizeAttachement(attachement.first, size);
    }
    if (_depthBuffer.first != nullptr)
        ResizeAttachement(_depthBuffer.first, size);
    if (_stencilBuffer.first != nullptr)
        ResizeAttachement(_stencilBuffer.first, size);
    _attachementsChanged = true;
}

std::shared_ptr<Textures::Texture> Framebuffer::Impl::GetColorBuffer(unsigned index)
{
    return _colorBuffers.at(index).first;
}

std::shared_ptr<Textures::Texture> Framebuffer::Impl::GetDepthBuffer()
{
    return _depthBuffer.first;
}

std::shared_ptr<Textures::Texture> Framebuffer::Impl::GetStencilBuffer()
{
    return _stencilBuffer.first;
}

void Framebuffer::Impl::AddColorBuffer(std::shared_ptr<Textures::Texture> buffer, unsigned mipLevel)
{
    SetColorBuffer(buffer, _colorBuffers.size());
}

void Framebuffer::Impl::Bind(OpenGL::Framebuffer::BindUsage usage)
{
    _SetupAttachements();
    glBindFramebuffer(OpenGL::GetEnum(usage), _handle);
}

void Framebuffer::Impl::Done(OpenGL::Framebuffer::BindUsage usage)
{
    BindDefault(usage);
}

void Framebuffer::Impl::BindDefault(OpenGL::Framebuffer::BindUsage usage)
{
    glBindFramebuffer(OpenGL::GetEnum(usage), 0);
}

void Framebuffer::Impl::_SetupAttachements()
{
    if (!_attachementsChanged)
        return;
    std::vector<GLenum> color_attachements;
    for (auto& attachement : _colorBuffers) {
        if (attachement.first != nullptr)
            attachement.first->Load();
    }
    //Distinct depth and stencil buffers are not allowed, use Depth24_Stencil8 texture format.
    if (_depthBuffer.first != nullptr && _stencilBuffer.first != nullptr)
        assert(_depthBuffer.first == _stencilBuffer.first);
    if (_depthBuffer.first != nullptr)
        _depthBuffer.first->Load();
    if (_stencilBuffer.first != nullptr)
        _stencilBuffer.first->Load();
    glBindFramebuffer(GL_FRAMEBUFFER, GetHandle());
    if (_depthBuffer.first != nullptr)
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthBuffer.first->GetImpl().GetHandle(), _depthBuffer.second);
    else
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
    if (_stencilBuffer.first != nullptr)
        glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, _stencilBuffer.first->GetImpl().GetHandle(), _stencilBuffer.second);
    else
        glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, 0, 0);
    for (auto i = 0u; i < _colorBuffers.size(); ++i) {
        auto attachement(_colorBuffers.at(i));
        if (attachement.first == nullptr) {
            color_attachements.push_back(GL_NONE);
            continue;
        }
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, attachement.first->GetImpl().GetHandle(), attachement.second);
        color_attachements.push_back(GL_COLOR_ATTACHMENT0 + i);
    }
    glDrawBuffers(color_attachements.size(), color_attachements.data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _attachementsChanged = false;
}

void Framebuffer::Impl::SetColorBuffer(std::shared_ptr<Textures::Texture> buffer, unsigned index, unsigned mipLevel)
{
    //_colorBuffers.push_back(std::pair(attachement, mipLevel));
    if (_colorBuffers.size() <= index)
        _colorBuffers.resize(index + 1);
    try {
        _attachementsChanged |= buffer != _colorBuffers.at(index).first;
        _attachementsChanged |= mipLevel != _colorBuffers.at(index).second;
        _colorBuffers.at(index) = std::make_pair(buffer, mipLevel);
    }
    catch (std::runtime_error& e) {
        throw std::runtime_error(std::string("Error while setting Color Buffer : ") + e.what());
    }
}

void Framebuffer::Impl::SetStencilBuffer(std::shared_ptr<Textures::Texture> buffer, unsigned mipLevel)
{
    _attachementsChanged |= buffer != _stencilBuffer.first;
    _attachementsChanged |= mipLevel != _stencilBuffer.second;
    _stencilBuffer = std::make_pair(buffer, mipLevel);
}

void Framebuffer::Impl::SetDepthBuffer(std::shared_ptr<Textures::Texture> buffer, unsigned mipLevel)
{
    _attachementsChanged |= buffer != _depthBuffer.first;
    _attachementsChanged |= mipLevel != _depthBuffer.second;
    _depthBuffer = std::make_pair(buffer, mipLevel);
}

void Framebuffer::Impl::BlitTo(std::shared_ptr<Framebuffer> to, glm::ivec2 src0, glm::ivec2 src1, glm::ivec2 dst0, glm::ivec2 dst1, BufferMask mask, Textures::Sampler::Filter filter)
{
    assert(to != nullptr);
    _SetupAttachements();
    to->GetImpl()._SetupAttachements();
    to->GetImpl().Bind(OpenGL::Framebuffer::BindUsage::Draw);
    Bind(OpenGL::Framebuffer::BindUsage::Read);
    glBlitFramebuffer(
        src0.x,
        src0.y,
        src1.x,
        src1.y,
        dst0.x,
        dst0.y,
        dst1.x,
        dst1.y,
        OpenGL::GetBitfield(mask),
        OpenGL::GetEnum(filter));
    Done(OpenGL::Framebuffer::BindUsage::Read);
}

void Framebuffer::Impl::BlitTo(std::shared_ptr<Framebuffer> to, BufferMask mask, Textures::Sampler::Filter filter)
{
    BlitTo(to, glm::ivec2(0), GetSize(), glm::ivec2(0), to->GetSize(), mask, filter);
}

void Framebuffer::Impl::BlitTo(std::shared_ptr<Core::Window> to, BufferMask mask, Textures::Sampler::Filter filter)
{
    assert(to != nullptr);
    glm::vec2 src0{ 0 };
    glm::vec2 dst0{ 0 };
    auto src1{ GetSize() };
    auto dst1{ to->GetSize() };

    _SetupAttachements();
    BindDefault(OpenGL::Framebuffer::BindUsage::Draw);
    Bind(OpenGL::Framebuffer::BindUsage::Read);
    glBlitFramebuffer(
        src0.x,
        src0.y,
        src1.x,
        src1.y,
        dst0.x,
        dst0.y,
        dst1.x,
        dst1.y,
        OpenGL::GetBitfield(mask),
        OpenGL::GetEnum(filter));
    Done(OpenGL::Framebuffer::BindUsage::Read);
}
}

unsigned OpenGL::GetEnum(Framebuffer::BindUsage usage)
{
    switch (usage)
    {
    case Framebuffer::BindUsage::None:
        return GL_FRAMEBUFFER;
    case Framebuffer::BindUsage::Draw:
        return GL_DRAW_FRAMEBUFFER;
    case Framebuffer::BindUsage::Read:
        return GL_READ_FRAMEBUFFER;
    default:
        throw std::runtime_error("Unknown Framebuffer::BindUsage");
    }
}

unsigned OpenGL::GetBitfield(TabGraph::Renderer::BufferMask mask)
{
    GLbitfield bitfield = 0;
    if ((mask & TabGraph::Renderer::BufferMask::ColorBits) == TabGraph::Renderer::BufferMask::ColorBits)
        bitfield |= GL_COLOR_BUFFER_BIT;
    if ((mask & TabGraph::Renderer::BufferMask::DepthBits) == TabGraph::Renderer::BufferMask::DepthBits)
        bitfield |= GL_COLOR_BUFFER_BIT;
    if ((mask & TabGraph::Renderer::BufferMask::StencilBits) == TabGraph::Renderer::BufferMask::StencilBits)
        bitfield |= GL_COLOR_BUFFER_BIT;
    return bitfield;
}

void OpenGL::Framebuffer::Bind(std::shared_ptr<TabGraph::Renderer::Framebuffer> fb, Framebuffer::BindUsage usage)
{
    if (fb == nullptr)
        TabGraph::Renderer::Framebuffer::Impl::BindDefault(usage);
    else
        fb->GetImpl().Bind(usage);
}
