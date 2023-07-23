#include <Renderer/OCRA/Shader.hpp>
#include <Renderer/OCRA/Renderer.hpp>

#include <OCRA/ShaderCompiler/Compiler.hpp>

namespace TabGraph::Renderer {
inline auto ToOCRA(const OCRA::ShaderCompiler::ShaderType& a_Type)
{
    switch (a_Type)
    {
    case OCRA::ShaderCompiler::ShaderType::Vertex:
        return OCRA::ShaderStageFlagBits::Vertex;
    case OCRA::ShaderCompiler::ShaderType::Geometry:
        return OCRA::ShaderStageFlagBits::Geometry;
    case OCRA::ShaderCompiler::ShaderType::Fragment:
        return OCRA::ShaderStageFlagBits::Fragment;
    case OCRA::ShaderCompiler::ShaderType::Compute:
        return OCRA::ShaderStageFlagBits::Compute;
    case OCRA::ShaderCompiler::ShaderType::TessControl:
        return OCRA::ShaderStageFlagBits::TessControl;
    case OCRA::ShaderCompiler::ShaderType::TessEval:
        return OCRA::ShaderStageFlagBits::TessEval;
    }
    return OCRA::ShaderStageFlagBits::None;
}
static inline auto GetTargetAPI()
{
    switch (OCRA_API_IMPL)
    {
    case OCRA_API_Vulkan:
        return OCRA::ShaderCompiler::TargetAPI::Vulkan;
    case OCRA_API_OpenGL:
        return OCRA::ShaderCompiler::TargetAPI::OpenGL;
    case OCRA_API_DirectX:
        return OCRA::ShaderCompiler::TargetAPI::DirectX;
    }
    return OCRA::ShaderCompiler::TargetAPI::Unknown;
}
Shader::Shader(
    const Renderer::Impl& a_Renderer,
    const std::vector<Stage>& a_Stages)
{
    const auto& compiler = a_Renderer.shaderCompiler;
    OCRA::ShaderCompiler::ShaderInfo shaderInfo;
    shaderInfo.targetAPI = GetTargetAPI();
    for (auto& stage : a_Stages) {
        auto shaderStage = ToOCRA(stage.type);
        shaderInfo.type = stage.type;
        shaderInfo.entryPoint = stage.entryPoint;
        shaderInfo.source = stage.source;
        auto compiledShader = OCRA::ShaderCompiler::Compile(compiler, shaderInfo);
        OCRA::PipelineShaderStage shaderStageInfo;
        shaderStageInfo.entryPoint = shaderInfo.entryPoint;
        shaderStageInfo.stage      = shaderStage;
        shaderStageInfo.module     = CreateShaderModule(a_Renderer.logicalDevice, { compiledShader.SPIRVBinary });
        stages.push_back(shaderStageInfo);
    }
}
}

