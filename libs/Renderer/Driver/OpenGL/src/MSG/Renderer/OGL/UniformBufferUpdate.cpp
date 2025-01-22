#include <MSG/Renderer/OGL/RAII/Buffer.hpp>
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