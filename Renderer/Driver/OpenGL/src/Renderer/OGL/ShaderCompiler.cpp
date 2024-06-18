#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/Shader.hpp>
#include <Renderer/OGL/ShaderCompiler.hpp>

#include <Renderer/ShaderLibrary.hpp>
#include <Renderer/ShaderPreprocessor.hpp>

#include <Tools/LazyConstructor.hpp>

#include <regex>

#include <GL/glew.h>

namespace TabGraph::Renderer {
ShaderCompiler::ShaderCompiler(Context& a_Context)
    : context(a_Context)
{
}

std::shared_ptr<RAII::Shader> ShaderCompiler::CompileShader(
    unsigned a_Stage,
    const std::string& a_Code)
{
    auto lazyConstructor = Tools::LazyConstructor([this, a_Stage, a_Code] {
        return RAII::MakePtr<RAII::Shader>(context, a_Stage, a_Code.data());
    });
    return shaderCache.GetOrCreate(a_Stage, a_Code, lazyConstructor);
}

unsigned GetShaderStage(const std::string& a_StageName)
{
    if (a_StageName == "Vertex")
        return GL_VERTEX_SHADER;
    else if (a_StageName == "Fragment")
        return GL_FRAGMENT_SHADER;
    else
        throw std::runtime_error("Unknown shader stage : " + a_StageName);
}

std::shared_ptr<RAII::Program> ShaderCompiler::CompileProgram(
    const std::string& a_Name,
    const ShaderLibrary::Program& a_Program)
{
    auto lazyConstructor = Tools::LazyConstructor([this, a_Program] {
        std::vector<std::shared_ptr<RAII::Shader>> shaders;
        for (auto& stage : a_Program.stages) {
            unsigned GLStage = GetShaderStage(stage.name);
            shaders.push_back(CompileShader(GLStage, stage.code));
        }
        return RAII::MakePtr<RAII::Program>(context, shaders);
    });
    return programCache.GetOrCreate(a_Name, lazyConstructor);
}

std::shared_ptr<RAII::Program> ShaderCompiler::CompileProgram(const std::string& a_Name)
{
    return CompileProgram(a_Name, ShaderLibrary::GetProgram(a_Name));
}
}
