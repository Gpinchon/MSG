#pragma once

#include <MSG/Renderer/ShaderLibrary.hpp>
#include <MSG/Tools/ObjectCache.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace std {
template <typename T>
struct hash;
template <>
struct hash<Msg::Renderer::ShaderLibrary::ProgramKeywords> {
    size_t operator()(Msg::Renderer::ShaderLibrary::ProgramKeywords const&) const;
};
}

namespace Msg::Renderer::ShaderLibrary {
struct Program;
}

namespace Msg {
class OGLContext;
class OGLShader;
class OGLProgram;
}

namespace Msg::Renderer {
using ShaderCacheKey  = Tools::ObjectCacheKey<unsigned, std::string>;
using ShaderCache     = Tools::ObjectCache<ShaderCacheKey, std::shared_ptr<OGLShader>>;
using ProgramCacheKey = Tools::ObjectCacheKey<std::string, ShaderLibrary::ProgramKeywords>;
using ProgramCache    = Tools::ObjectCache<ProgramCacheKey, std::shared_ptr<OGLProgram>>;
class ShaderCompiler {
public:
    ShaderCompiler(OGLContext& a_Context);
    /**
     * @brief compile a custom shader
     */
    std::shared_ptr<OGLShader> CompileShader(
        unsigned a_Stage,
        const std::string& a_Code);
    /**
     * @brief compile a custom program
     */
    std::shared_ptr<OGLProgram> CompileProgram(
        const std::string& a_Name,
        const ShaderLibrary::Program& a_Program);
    /**
     * @brief compile a program from the shader library
     */
    std::shared_ptr<OGLProgram> CompileProgram(
        const std::string& a_Name,
        const ShaderLibrary::ProgramKeywords& a_Keywords = {});
    /**
     * @brief precompiles the whole shader library
     */
    void PrecompileLibrary();
    OGLContext& context;
    ProgramCache programCache;
    ShaderCache shaderCache;
};
}
