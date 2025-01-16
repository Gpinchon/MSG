#include <Renderer/OCRA/Material.hpp>
#include <Renderer/OCRA/Renderer.hpp>

#include <OCRA/ShaderCompiler/Compiler.hpp>

namespace TabGraph::Renderer {
Material::Material(
    const Renderer::Impl& a_Renderer,
    const SG::Material& a_Material)
{
    //OCRA::DescriptorSetBinding binding;
    //binding.binding = 0;
    //binding.count = 1;
    //binding.stageFlags = OCRA::ShaderStageFlagBits::Vertex;
    //binding.type = OCRA::DescriptorType::UniformBuffer;
    //bindings.push_back(binding);
}
}