#include <Renderer/OGL/RAII/Shader.hpp>
#include <Renderer/OGL/ShaderCompiler.hpp>

#include <Renderer/ShaderPreprocessor.hpp>

#include <Tools/LazyConstructor.hpp>

#include <regex>

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
        return RAII::MakePtr<RAII::Shader>(context, a_Stage, ShaderPreprocessor {}.ExpandCode(a_Code).c_str());
    });
    return *GetOrCreate(a_Stage, a_Code, lazyConstructor);
}
}
