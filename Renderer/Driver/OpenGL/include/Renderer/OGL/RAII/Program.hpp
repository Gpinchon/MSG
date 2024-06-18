#pragma once

#include <memory>
#include <vector>

namespace TabGraph::Renderer::RAII {
struct Shader;
}

namespace TabGraph::Renderer::RAII {
struct Program {
    Program(const std::vector<std::shared_ptr<Shader>>& a_Shaders);
    ~Program();
    operator unsigned() const { return handle; }
    const unsigned handle;
};
}
