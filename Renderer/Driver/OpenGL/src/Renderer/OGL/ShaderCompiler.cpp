#include <Renderer/OGL/ShaderCompiler.hpp>
#include <Renderer/OGL/RAII/Shader.hpp>

#include <Tools/LazyConstructor.hpp>

namespace TabGraph::Renderer {
RAII::Shader& ShaderCompiler::CompileShader(
    unsigned a_Stage,
    const std::string& a_Code)
{
    auto lazyConstructor = Tools::LazyConstructor([this, a_Stage, a_Code] {
        return RAII::MakeWrapper<RAII::Shader>(context, a_Stage, a_Code.c_str());
    });
    return *GetOrCreate(a_Stage, a_Code, lazyConstructor);
}
}