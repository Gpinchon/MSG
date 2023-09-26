#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <Tools/ObjectCache.hpp>

#include <string>

namespace TabGraph::Renderer::RAII {
struct Context;
struct Shader;
}

namespace TabGraph::Renderer {
using ShaderCacheKey = Tools::ObjectCacheKey<unsigned, std::string>;
struct ShaderCompiler : private Tools::ObjectCache<ShaderCacheKey, RAII::Wrapper<RAII::Shader>> {
    ShaderCompiler(RAII::Context& a_Context)
        : context(a_Context)
    {
    }
    RAII::Shader& CompileShader(
        unsigned a_Stage,
        const std::string& a_Code);
    RAII::Context& context;
};
}
