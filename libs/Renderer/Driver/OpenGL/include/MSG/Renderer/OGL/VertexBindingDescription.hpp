#pragma once

#include <MSG/Renderer/OGL/RAII/Wrapper.hpp>

namespace MSG::Renderer::RAII {
class Buffer;
}

namespace MSG::Renderer {
struct VertexBindingDescription {
    unsigned index  = 0;
    unsigned offset = 0;
    unsigned stride = 0;
    std::shared_ptr<RAII::Buffer> buffer;
};
}
