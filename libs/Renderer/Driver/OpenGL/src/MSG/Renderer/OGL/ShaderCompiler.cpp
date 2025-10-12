#include <MSG/Debug.hpp>
#include <MSG/OGLProgram.hpp>
#include <MSG/OGLShader.hpp>
#include <MSG/Renderer/OGL/ShaderCompiler.hpp>
#include <MSG/Renderer/ShaderLibrary.hpp>
#include <MSG/Renderer/ShaderPreprocessor.hpp>
#include <MSG/Tools/ArrayHasher.hpp>
#include <MSG/Tools/LazyConstructor.hpp>
#include <MSG/Tools/ScopedTimer.hpp>

#include <iostream>

#include <GL/glew.h>

size_t std::hash<Msg::Renderer::ShaderLibrary::ProgramKeywords>::operator()(Msg::Renderer::ShaderLibrary::ProgramKeywords const& a_Keywords) const
{
    return Msg::Tools::HashArray(a_Keywords);
}

namespace Msg::Renderer {
ShaderCompiler::ShaderCompiler(OGLContext& a_Context)
    : context(a_Context)
{
}

std::shared_ptr<OGLShader> ShaderCompiler::CompileShader(
    unsigned a_Stage,
    const std::string& a_Code)
{
    auto lazyConstructor = Tools::LazyConstructor([this, a_Stage, a_Code] {
        auto timer  = Tools::ScopedTimer("Compiling shader");
        auto shader = std::make_shared<OGLShader>(context, a_Stage, a_Code.data());
        return shader;
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
    auto lazyConstructor = Tools::LazyConstructor([this, a_Name, a_Program] {
        std::vector<std::shared_ptr<OGLShader>> shaders;
        auto timer = Tools::ScopedTimer("Compiling program " + a_Name);
        for (auto& stage : a_Program.stages)
            shaders.push_back(CompileShader(GetShaderStage(stage.name), stage.code));
        for (auto& shader : shaders) {
            if (!shader->GetStatus())
                errorFatal("Compilation error : " + a_Name + " " + shader->GetLog());
        }
        auto program = std::make_shared<OGLProgram>(context, shaders);
        if (!program->GetStatus())
            errorFatal("Compilation error : " + a_Name + " " + program->GetLog());
        return program;
    });
    return programCache.GetOrCreate(a_Name, a_Program.keywords, lazyConstructor);
}

std::shared_ptr<OGLProgram> ShaderCompiler::CompileProgram(
    const std::string& a_Name,
    const ShaderLibrary::ProgramKeywords& a_Keywords)
{
    return CompileProgram(a_Name, ShaderLibrary::GetProgram(a_Name, a_Keywords));
}

void ShaderCompiler::PrecompileLibrary()
{
    auto timer = Tools::ScopedTimer("Precompiling shaders library");
    for (auto& program : ShaderLibrary::GetProgramsLibrary()) {
        for (auto& variant : program.second)
            CompileProgram(program.first, variant);
    }
}
}
