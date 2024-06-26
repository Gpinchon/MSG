#pragma once

#include <cstdint>
#include <memory>

namespace TabGraph::Renderer::RAII {
struct Buffer;
}

namespace TabGraph::Renderer {
struct Context;
}

namespace TabGraph::Renderer {
class ShaderStorageBuffer {
    ShaderStorageBuffer(
        Context& a_Context,
        const uint32_t& a_Offset,
        const uint32_t& a_Size);
    uint32_t offset                      = 0;
    uint32_t size                        = 0;
    std::shared_ptr<RAII::Buffer> buffer = nullptr;
};
}