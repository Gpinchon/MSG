#include <Renderer/OGL/ShaderCompiler.hpp>

#include <Tools/LazyConstructor.hpp>

namespace TabGraph::Renderer {
RAII::Shader& ShaderCompiler::CompileShader(
    unsigned a_Stage,
    const std::string& a_Code)
{
    auto lazyConstructor = Tools::LazyConstructor([a_Stage, a_Code] {
        return std::move(RAII::Shader(a_Stage, a_Code.c_str()));
    });
    return GetOrCreate(a_Stage, a_Code, lazyConstructor);
}
}