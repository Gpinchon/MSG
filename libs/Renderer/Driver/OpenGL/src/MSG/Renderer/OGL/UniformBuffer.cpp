#include <MSG/OGLBuffer.hpp>
#include <MSG/Renderer/OGL/UniformBuffer.hpp>

#include <GL/glew.h>

namespace MSG::Renderer {
UniformBuffer::UniformBuffer(OGLContext& a_Context, const size_t& a_Size, const std::byte* a_Data)
    : buffer(std::make_shared<OGLBuffer>(a_Context, a_Size, a_Data, GL_DYNAMIC_STORAGE_BIT))
{
}
}
