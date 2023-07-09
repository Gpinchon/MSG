#include <Renderer/OCRA/Component/MeshData.hpp>

#include <Renderer/OCRA/Material.hpp>
#include <Renderer/OCRA/Primitive.hpp>
#include <Renderer/OCRA/Renderer.hpp>

#include <SG/Component/Mesh.hpp>

#include <Tools/LazyConstructor.hpp>

namespace TabGraph::Renderer::Component {
MeshData::MeshData(Renderer::Impl* a_Renderer, const SG::Component::Mesh& a_Mesh)
{
    OCRA::CreatePipelineGraphicsInfo pipelineInfo;
    OCRA::ViewPort viewPort;
    viewPort.rect.extent = { 256, 256 };
    pipelineInfo.viewPortState.viewPorts = { viewPort };
    pipelineInfo.descriptorUpdate = OCRA::DescriptorUpdate::Push;
    for (const auto& it : a_Mesh.primitives) {
        {
            const auto& primitive = it.first;
            auto [it, success]    = a_Renderer->primitives.try_emplace(primitive.get(),
                Tools::LazyConstructor(
                    [
                        &a_Renderer, &primitive
                    ] () {
                        return std::make_shared<Primitive>(*a_Renderer, *primitive);
                    }));
            auto& newPrimitive = it->second;
            primitives.push_back(newPrimitive);
            pipelineInfo.inputAssemblyState.topology = newPrimitive->topology;
            pipelineInfo.vertexInputState = newPrimitive->vertexBuffer.GetVertexInput();
            pipelineInfo.shaderPipelineState.stages.push_back(newPrimitive->vertexShader);
            pipelineInfo.bindings.insert(pipelineInfo.bindings.end(), newPrimitive->bindings.begin(), newPrimitive->bindings.end());
        }
        {
            const auto& material = it.second;
            auto [it, success]   = a_Renderer->materials.try_emplace(material.get(),
                Tools::LazyConstructor(
                    [
                        &a_Renderer, &material
                    ] () {
                        return std::make_shared<Material>(*a_Renderer, *material);
                    }));
            auto& newMaterial = it->second;
            materials.push_back(newMaterial);
            pipelineInfo.shaderPipelineState.stages.push_back(newMaterial->fragmentShader);
            pipelineInfo.bindings.insert(pipelineInfo.bindings.end(), newMaterial->bindings.begin(), newMaterial->bindings.end());
        }
        graphicsPipelines.push_back(OCRA::Device::CreatePipelineGraphics(a_Renderer->logicalDevice, pipelineInfo));
    }
}
}
