#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLContext.hpp>

#include <GL/glew.h>

namespace Msg {
static inline auto CreateBuffer(OGLContext& a_OGLContext)
{
    unsigned handle = 0;
    ExecuteOGLCommand(a_OGLContext, [&handle] { glCreateBuffers(1, &handle); }, true);
    return handle;
}

OGLBuffer::OGLBuffer(OGLContext& a_OGLContext, const size_t& a_Size, const void* a_Data, const unsigned& a_Flags)
    : handle(CreateBuffer(a_OGLContext))
    , size(a_Size)
    , context(a_OGLContext)
{
    ExecuteOGLCommand(a_OGLContext, [&] { glNamedBufferStorage(handle, size, a_Data, a_Flags); }, true);
}
OGLBuffer::~OGLBuffer()
{
    ExecuteOGLCommand(context, [handle = handle] { glDeleteBuffers(1, &handle); });
}
}
