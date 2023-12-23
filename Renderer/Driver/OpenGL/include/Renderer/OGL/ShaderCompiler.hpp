#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <Tools/ObjectCache.hpp>

#include <string>

namespace TabGraph::Renderer {
struct Context;
}

namespace TabGraph::Renderer::RAII {
struct Shader;
}

namespace TabGraph::Renderer {
using ShaderCacheKey = Tools::ObjectCacheKey<unsigned, std::string>;
struct ShaderCompiler : private Tools::ObjectCache<ShaderCacheKey, std::shared_ptr<RAII::Shader>> {
    ShaderCompiler(Context& a_Context);
    RAII::Shader& CompileShader(
        unsigned a_Stage,
        const std::string& a_Code);
    Context& context;
};
}
