#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <Tools/ObjectCache.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace TabGraph::Renderer::ShaderLibrary {
struct Program;
}

namespace TabGraph::Renderer {
struct Context;
}

namespace TabGraph::Renderer::RAII {
struct Shader;
struct Program;
}

namespace TabGraph::Renderer {
using ShaderCacheKey  = Tools::ObjectCacheKey<unsigned, std::string>;
using ProgramCacheKey = Tools::ObjectCacheKey<std::string>;
using ProgramCache    = Tools::ObjectCache<ProgramCacheKey, std::shared_ptr<RAII::Program>>;
struct ShaderCompiler : private Tools::ObjectCache<ShaderCacheKey, std::shared_ptr<RAII::Shader>> {
    ShaderCompiler(Context& a_Context);
    /**
     * @brief compile a custom shader
     */
    RAII::Shader& CompileShader(
        unsigned a_Stage,
        const std::string& a_Code);
    /**
     * @brief compile a custom program
     */
    std::shared_ptr<RAII::Program> CompileProgram(
        const std::string& a_Name,
        const ShaderLibrary::Program& a_Program);
    /**
     * @brief compile a program from the shader library
     */
    std::shared_ptr<RAII::Program> CompileProgram(const std::string& a_Name);
    Context& context;
    ProgramCache programCache;
};
}
