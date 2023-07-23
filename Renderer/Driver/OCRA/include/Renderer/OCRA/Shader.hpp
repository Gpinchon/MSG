#pragma once

#include <Renderer/Handles.hpp>

#include <OCRA/OCRA.hpp>

#include <OCRA/ShaderCompiler/Compiler.hpp>

#include <string>
#include <vector>

namespace TabGraph::Renderer {
struct Shader {
    struct Stage {
        OCRA::ShaderCompiler::ShaderType type;
        std::string entryPoint;
        std::string source;
    };
    Shader(const Renderer::Impl& a_Renderer, const std::vector<Stage>& a_Stages);
    std::vector<OCRA::PipelineShaderStage> stages;
};
}