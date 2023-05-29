/*
 * @Author: gpinchon
 * @Date:   2020-06-18 13:31:08
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-01-14 16:24:41
 */
#include <SG/Core/Buffer/Accessor.hpp>
#include <SG/Core/Buffer/View.hpp>

namespace TabGraph::SG {
size_t BufferAccessor::s_bufferAccessorNbr      = 0u;
size_t BufferAccessor::s_typedBufferAccessorNbr = 0u;
}
