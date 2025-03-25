#include <MSG/OGLProgram.hpp>
#include <MSG/OGLShader.hpp>
#include <MSG/Renderer/OGL/ShaderCompiler.hpp>
#include <MSG/Renderer/ShaderLibrary.hpp>
#include <MSG/Renderer/ShaderPreprocessor.hpp>
#include <MSG/Tools/LazyConstructor.hpp>

#include <regex>

#include <GL/glew.h>

namespace MSG::Renderer {
ShaderCompiler::ShaderCompiler(OGLContext& a_Context)
    : context(a_Context)
{
}

std::shared_ptr<OGLShader> ShaderCompiler::CompileShader(
    unsigned a_Stage,
    const std::string& a_Code)
{
    auto lazyConstructor = Tools::LazyConstructor([this, a_Stage, a_Code] {
        return std::make_shared<OGLShader>(context, a_Stage, a_Code.data());
    });
    return shaderCache.GetOrCreate(a_Stage, a_Code, lazyConstructor);
}

unsigned GetShaderStage(const ShaderLibrary::StageName& a_StageName)
{
    switch (a_StageName) {
    case ShaderLibrary::StageName::Vertex:
        return GL_VERTEX_SHADER;
    case ShaderLibrary::StageName::Fragment:
        return GL_FRAGMENT_SHADER;
    case ShaderLibrary::StageName::Compute:
        return GL_COMPUTE_SHADER;
    default:
        throw std::runtime_error("Unknown shader stage");
    }
    return GL_NONE;
}

std::shared_ptr<OGLProgram> ShaderCompiler::CompileProgram(
    const std::string& a_Name,
    const ShaderLibrary::Program& a_Program)
{
    auto lazyConstructor = Tools::LazyConstructor([this, a_Program] {
        std::vector<std::shared_ptr<OGLShader>> shaders;
        for (auto& stage : a_Program.stages) {
            unsigned GLStage = GetShaderStage(stage.name);
            shaders.push_back(CompileShader(GLStage, stage.code));
        }
        return std::make_shared<OGLProgram>(context, shaders);
    });
    return programCache.GetOrCreate(a_Name, lazyConstructor);
}

std::shared_ptr<OGLProgram> ShaderCompiler::CompileProgram(const std::string& a_Name)
{
    return CompileProgram(a_Name, ShaderLibrary::GetProgram(a_Name));
}

void ShaderCompiler::PrecompileLibrary()
{
    for (auto& program : ShaderLibrary::GetProgramsLibrary()) {
        for (auto& variant : program.second)
            CompileProgram(program.first, variant);
    }
}
}
