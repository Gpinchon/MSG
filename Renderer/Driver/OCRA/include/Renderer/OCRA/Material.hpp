#pragma once

#include <Renderer/Handles.hpp>

#include <OCRA/Structs.hpp>

#include <memory>

namespace TabGraph::SG {
class Material;
}

namespace TabGraph::Renderer {
struct Material {
    Material(
        const Renderer::Impl& a_Renderer,
        const SG::Material& a_Material);
    OCRA::PipelineShaderStage fragmentShader;
    std::vector<OCRA::DescriptorSetBinding> bindings;
};
}
