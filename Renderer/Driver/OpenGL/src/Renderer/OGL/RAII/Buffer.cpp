#include <Renderer/OGL/RAII/Buffer.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer::RAII {
static inline auto CreateBuffer()
{
    unsigned handle = 0;
    glCreateBuffers(1, &handle);
    return handle;
}
Buffer::Buffer(const size_t& a_Size, const void* a_Data, const unsigned& a_Flags)
    : handle(CreateBuffer())
    , size(a_Size)
{
    glNamedBufferStorage(handle, a_Size, a_Data, a_Flags);
}
Buffer::~Buffer()
{
    glDeleteBuffers(1, &handle);
}
}
