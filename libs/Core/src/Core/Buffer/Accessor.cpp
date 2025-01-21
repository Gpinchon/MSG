/*
 * @Author: gpinchon
 * @Date:   2020-06-18 13:31:08
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-01-14 16:24:41
 */
#include <Core/Buffer/Accessor.hpp>
#include <Core/Buffer/View.hpp>

namespace MSG::Core {
size_t BufferAccessor::s_bufferAccessorNbr      = 0u;
size_t BufferAccessor::s_typedBufferAccessorNbr = 0u;
}
