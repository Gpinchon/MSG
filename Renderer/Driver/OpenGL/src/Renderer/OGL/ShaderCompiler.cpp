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
RAII::Shader& ShaderCompiler::CompileShader(
    unsigned a_Stage,
    const std::string& a_Code)
{
    auto lazyConstructor = Tools::LazyConstructor([this, a_Stage, a_Code] {
        return RAII::MakePtr<RAII::Shader>(context, a_Stage, ShaderPreprocessor {}.ExpandCode(a_Code).data());
    });
    return *GetOrCreate(a_Stage, a_Code, lazyConstructor);
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

std::vector<RAII::Shader*> ShaderCompiler::CompileProgram(const ShaderLibrary::Program& a_Program)
{
    std::vector<RAII::Shader*> shaders;
    for (auto& stage : a_Program.stages) {
        unsigned GLStage     = GetShaderStage(stage.name);
        auto lazyConstructor = Tools::LazyConstructor([this, GLStage, &code = stage.code] {
            return RAII::MakePtr<RAII::Shader>(context, GLStage, code.data());
        });
        shaders.push_back(&*GetOrCreate(GLStage, stage.code, lazyConstructor));
    }
    return shaders;
}
}
