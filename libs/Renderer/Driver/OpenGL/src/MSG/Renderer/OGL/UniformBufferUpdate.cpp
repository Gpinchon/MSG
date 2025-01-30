#include <MSG/OGLBuffer.hpp>
#include <MSG/Renderer/OGL/UniformBufferUpdate.hpp>

#include <GL/glew.h>

namespace MSG::Renderer {
void UniformBufferUpdate::operator()() const
{
    glNamedBufferSubData(
        *_buffer, _offset,
        _size, _data.get());
}
}