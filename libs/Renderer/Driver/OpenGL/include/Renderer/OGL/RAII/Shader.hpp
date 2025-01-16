#pragma once

namespace TabGraph::Renderer::RAII {
struct Shader {
    Shader(
        const unsigned a_Stage,
        const char* a_Code);
    ~Shader();
    operator unsigned() const { return handle; }
    const unsigned handle;
    const unsigned stage;
};
}
