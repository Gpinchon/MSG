/*
* @Author: gpinchon
* @Date:   2021-05-11 00:56:01
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-11 13:52:54
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Renderer/Framebuffer.hpp>
#include <Driver/OpenGL/ObjectHandle.hpp>

#include <memory>
#include <vector>
#include <glm/vec2.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Core {
class Window;
}
namespace Textures {
class Texture;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class Declaration
////////////////////////////////////////////////////////////////////////////////
namespace OpenGL::Framebuffer {
enum class BindUsage {
    None,
    Draw,
    Read
};
}

namespace TabGraph::Renderer {
class Framebuffer::Impl {
public:
    using Handle = OpenGL::ObjectHandle;
    Impl(const Framebuffer& framebuffer);
    ~Impl();
    Handle GetHandle();
    glm::ivec2 GetSize() const;
    void SetSize(const glm::ivec2 size);
    std::shared_ptr<Textures::Texture> GetColorBuffer(unsigned index);
    std::shared_ptr<Textures::Texture> GetDepthBuffer();
    std::shared_ptr<Textures::Texture> GetStencilBuffer();
    void AddColorBuffer(std::shared_ptr<Textures::Texture> buffer, unsigned mipLevel = 0);
    void SetColorBuffer(std::shared_ptr<Textures::Texture> buffer, unsigned color_attachement = 0, unsigned mipLevel = 0);
    void SetStencilBuffer(std::shared_ptr<Textures::Texture> buffer, unsigned mipLevel = 0);
    void SetDepthBuffer(std::shared_ptr<Textures::Texture> buffer, unsigned mipLevel = 0);
    void BlitTo(std::shared_ptr<Framebuffer> to,
        glm::ivec2 src0, glm::ivec2 src1,
        glm::ivec2 dst0, glm::ivec2 dst1,
        BufferMask mask = BufferMask::ColorBits | BufferMask::DepthBits | BufferMask::StencilBits,
        Textures::Sampler::Filter filter = Textures::Sampler::Filter::Nearest);
    void BlitTo(std::shared_ptr<Framebuffer> to,
        BufferMask mask = BufferMask::ColorBits | BufferMask::DepthBits | BufferMask::StencilBits,
        Textures::Sampler::Filter filter = Textures::Sampler::Filter::Nearest);
    void BlitTo(std::shared_ptr<Core::Window> to,
        BufferMask mask = BufferMask::ColorBits | BufferMask::DepthBits | BufferMask::StencilBits,
        Textures::Sampler::Filter filter = Textures::Sampler::Filter::Nearest);

    void Bind(OpenGL::Framebuffer::BindUsage usage);
    void Done(OpenGL::Framebuffer::BindUsage usage);
    static void BindDefault(OpenGL::Framebuffer::BindUsage usage);

private:
    void _SetupAttachements();
    const Framebuffer& _framebuffer;
    Handle _handle { 0 };
    std::vector<std::pair<std::shared_ptr<Textures::Texture>, unsigned>> _colorBuffers;
    std::pair<std::shared_ptr<Textures::Texture>, unsigned> _depthBuffer;
    std::pair<std::shared_ptr<Textures::Texture>, unsigned> _stencilBuffer;
    glm::ivec2 _size { 0, 0 };
    bool _attachementsChanged { true };
};
}

namespace OpenGL {
unsigned GetEnum(Framebuffer::BindUsage usage);
unsigned GetBitfield(TabGraph::Renderer::BufferMask mask);
namespace Framebuffer {
    void Bind(std::shared_ptr<TabGraph::Renderer::Framebuffer> fb, Framebuffer::BindUsage usage = Framebuffer::BindUsage::Draw);
}
}
