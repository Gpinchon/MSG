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
    const std::string& a_Code,
    const ShaderDefines& a_Defines)
{
    std::string finalCode;
    for (auto& define : a_Defines) {
        finalCode += "#define " + define.first + " " + define.second + ";\n";
    }
    finalCode += a_Code;
    auto lazyConstructor = Tools::LazyConstructor([this, a_Stage, finalCode] {
        return RAII::MakePtr<RAII::Shader>(context, a_Stage, ShaderPreprocessor {}.ExpandCode(finalCode).data());
    });
    return *GetOrCreate(a_Stage, a_Code, lazyConstructor);
}
}
