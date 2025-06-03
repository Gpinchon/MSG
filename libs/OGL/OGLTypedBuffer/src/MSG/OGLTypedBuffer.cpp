#include <MSG/OGLContext.hpp>
#include <MSG/OGLTypedBuffer.hpp>

#include <GL/glew.h>
#include <vector>

void MSG::UpdateOGLTypedBuffer(const OGLBuffer& a_Buffer, const size_t& a_Offset, const size_t& a_Size, void* a_Data)
{
    auto func = [handle = a_Buffer.handle, offset = a_Offset, size = a_Size, data = std::vector<std::byte> { (std::byte*)a_Data, (std::byte*)a_Data + a_Size }] {
        glInvalidateBufferSubData(handle, offset, size);
        glNamedBufferSubData(
            handle, offset,
            size, data.data());
    };
    MSG::ExecuteOGLCommand(a_Buffer.context, func);
}

void MSG::ReadOGLTypedBuffer(const OGLBuffer& a_Buffer, const size_t& a_Offset, const size_t& a_Size, void* a_Data)
{
    auto func = [handle = a_Buffer.handle, offset = a_Offset, size = a_Size, a_Data] {
        glGetNamedBufferSubData(
            handle,
            offset, size,
            a_Data);
    };
    MSG::ExecuteOGLCommand(a_Buffer.context, func, true);
}
