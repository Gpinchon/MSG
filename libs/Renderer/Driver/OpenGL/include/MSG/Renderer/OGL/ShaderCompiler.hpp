#pragma once

#include <MSG/Renderer/OGL/ObjectRepertory.hpp>
#include <MSG/Renderer/ShaderLibrary.hpp>
#include <MSG/Tools/ObjectCache.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Msg::Renderer::ShaderLibrary {
struct Program;
}

namespace Msg {
class OGLContext;
class OGLShader;
class OGLProgram;
}

namespace Msg::Renderer {
using ShaderCacheKey = Tools::ObjectCacheKey<unsigned, std::string>;
using ShaderCache    = Tools::ObjectCache<ShaderCacheKey, std::shared_ptr<OGLShader>>;
using ProgramCache   = ObjectRepertory<std::shared_ptr<OGLProgram>>;

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
    std::shared_ptr<OGLProgram> CompileProgram(const std::string& a_Name);
    /**
     * @brief compiles a program from the library with the specified keywords
     *
     * @tparam Args
     * @param a_Name
     * @param a_Keywords
     * @return std::shared_ptr<OGLProgram>
     */
    template <std::same_as<ShaderLibrary::ProgramKeyword>... Args>
    std::shared_ptr<OGLProgram> CompileProgram(const std::string& a_Name, Args&&... a_Keywords);
    /**
     * @brief precompiles the whole shader library
     */
    void PrecompileLibrary();

private:
    std::shared_ptr<OGLProgram> _CompileProgram(const std::string& a_Name, const ShaderLibrary::Program& a_Program);
    OGLContext& _context;
    ProgramCache _programCache;
    ShaderCache _shaderCache;
};

template <std::same_as<ShaderLibrary::ProgramKeyword>... Args>
std::shared_ptr<OGLProgram> ShaderCompiler::CompileProgram(const std::string& a_Name, Args&&... a_Keywords)
{
    auto* repertory = &_programCache[a_Name];
    auto process    = [&repertory](const auto& a_Keyword) {
        repertory = &(*repertory)[a_Keyword.second];
    };
    (process(std::forward<Args>(a_Keywords)), ...);
    if (**repertory == nullptr) {
        auto& program = ShaderLibrary::GetProgram(a_Name, { std::forward<Args>(a_Keywords)... });
        *repertory    = _CompileProgram(a_Name, program);
    }
    return **repertory;
}
}
