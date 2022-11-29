#include <SG/Buffer/Buffer.hpp>

namespace TabGraph::SG {
Buffer::Buffer() : Inherit() {
    static size_t s_BufferNbr = 0;
    SetName("Buffer_" + std::to_string(++s_BufferNbr));
}
}