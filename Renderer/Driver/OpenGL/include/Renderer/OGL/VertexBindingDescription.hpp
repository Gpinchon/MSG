#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

namespace TabGraph::Renderer::RAII {
struct Buffer;
}

namespace TabGraph::Renderer {
struct VertexBindingDescription { 
    unsigned index;
    unsigned offset;
    unsigned stride;
    RAII::Wrapper<RAII::Buffer> buffer;
};
}