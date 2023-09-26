#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>

#include <GL/glew.h>
#include <algorithm>

namespace TabGraph::Renderer::RAII {
auto CreateFramebuffer()
{
    unsigned handle = 0;
    glCreateFramebuffers(1, &handle);
    return handle;
}

FrameBuffer::FrameBuffer(const unsigned& a_Width, const unsigned& a_Height)
    : handle(CreateFramebuffer())
    , width(a_Width)
    , height(a_Height)
{
    glNamedFramebufferParameteri(handle, GL_FRAMEBUFFER_DEFAULT_WIDTH, width);
    glNamedFramebufferParameteri(handle, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &handle);
}
}
