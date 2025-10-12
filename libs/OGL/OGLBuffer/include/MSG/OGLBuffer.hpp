#pragma once

#include <cstddef>

namespace Msg {
class OGLContext;
}

namespace Msg {
class OGLBuffer {
public:
    /**
     * @brief Creates a buffer and allocates storage
     * @param a_Size the size of the storage
     * @param a_Data the data of the buffer
     * @param a_Flags the flags that will be used for the storage
     */
    OGLBuffer(OGLContext& a_OGLContext, const size_t& a_Size, const void* a_Data, const unsigned& a_Flags);
    ~OGLBuffer();
    operator unsigned() const { return handle; }
    const unsigned handle;
    const unsigned size;
    OGLContext& context;
};
}
