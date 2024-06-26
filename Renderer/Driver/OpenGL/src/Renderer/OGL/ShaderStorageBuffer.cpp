#include <Renderer/OGL/ShaderStorageBuffer.hpp>

#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer {
ShaderStorageBuffer::ShaderStorageBuffer(
    Context& a_Context,
    const uint32_t& a_Offset,
    const uint32_t& a_Size)
    : offset(a_Offset)
    , size(a_Size)
    , buffer(RAII::MakePtr<RAII::Buffer>(a_Context, size, nullptr, GL_DYNAMIC_STORAGE_BIT))
{
}
}