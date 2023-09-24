#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <vector>

namespace TabGraph::Renderer::RAII {
struct Shader;
}

namespace TabGraph::Renderer::RAII {
struct Program {
    Program(
        const std::vector<Wrapper<Shader>>& a_Shaders);
    ~Program();
    operator unsigned() const { return handle; }
    const unsigned handle;
};
}
