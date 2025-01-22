#pragma once

#include <memory>
#include <vector>

namespace MSG::Renderer::RAII {
class Shader;
}

namespace MSG::Renderer::RAII {
class Program {
public:
    Program(const std::vector<std::shared_ptr<Shader>>& a_Shaders);
    ~Program();
    operator unsigned() const { return handle; }
    const unsigned handle;
};
}
