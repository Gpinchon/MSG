#include <MSG/OGLBuffer.hpp>
#include <MSG/Renderer/OGL/UniformBufferUpdate.hpp>

#include <GL/glew.h>

namespace MSG::Renderer {
void UniformBufferUpdate::operator()() const
{
    glInvalidateBufferSubData(*_buffer, _offset, _size);
    glNamedBufferSubData(
        *_buffer, _offset,
        _size, _data.get());
}
}