#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

namespace TabGraph::Renderer::RAII {
struct Buffer;
}

namespace TabGraph::Renderer {
struct VertexBindingDescription {
    unsigned index  = 0;
    unsigned offset = 0;
    unsigned stride = 0;
    std::shared_ptr<RAII::Buffer> buffer;
};
}
