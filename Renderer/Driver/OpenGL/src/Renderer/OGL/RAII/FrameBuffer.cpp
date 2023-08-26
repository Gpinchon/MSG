#include <Renderer/OGL/RAII/FrameBuffer.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer::RAII {
FrameBuffer::FrameBuffer()
{
    glCreateFramebuffers(1, &handle);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &handle);
}
}
