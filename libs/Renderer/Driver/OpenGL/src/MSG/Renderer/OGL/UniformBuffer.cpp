#include <MSG/Renderer/OGL/RAII/Buffer.hpp>
#include <MSG/Renderer/OGL/UniformBuffer.hpp>

#include <GL/glew.h>

namespace MSG::Renderer {
UniformBuffer::UniformBuffer(OGLContext& a_Context, const size_t& a_Size, const std::byte* a_Data)
    : buffer(RAII::MakePtr<RAII::Buffer>(a_Context, a_Size, a_Data, GL_DYNAMIC_STORAGE_BIT))
{
}
}
