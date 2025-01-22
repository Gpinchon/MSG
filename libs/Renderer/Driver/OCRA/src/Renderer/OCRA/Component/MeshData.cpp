#include <MSG/Renderer/OCRA/Component/MeshData.hpp>

#include <MSG/Renderer/OCRA/Material.hpp>
#include <MSG/Renderer/OCRA/Primitive.hpp>
#include <MSG/Renderer/OCRA/Renderer.hpp>

#include <MSG/Mesh.hpp>

#include <MSG/Tools/LazyConstructor.hpp>

namespace MSG::Renderer::Component {
MeshData::MeshData(Renderer::Impl* a_Renderer, const Mesh& a_Mesh)
{
    OCRA::ViewPort viewPort;
    viewPort.rect.extent = { 256, 256 };
    for (const auto& it : a_Mesh.primitives) {
        OCRA::CreatePipelineGraphicsInfo pipelineInfo;
        pipelineInfo.viewPortState.viewPorts = { viewPort };
        pipelineInfo.descriptorUpdate        = OCRA::DescriptorUpdate::Push;
        {
            const auto& primitive = it.first;
            auto [it, success]    = a_Renderer->primitives.try_emplace(primitive.get(),
                   Tools::LazyConstructor(
                    [&a_Renderer, &primitive]() {
                        return std::make_shared<Primitive>(*a_Renderer, *primitive);
                    }));
            auto& newPrimitive    = it->second;
            primitives.push_back(newPrimitive);
            pipelineInfo.inputAssemblyState.topology = newPrimitive->topology;
            pipelineInfo.vertexInputState            = newPrimitive->vertexBuffer.GetVertexInput();
        }
        {
            const auto& material = it.second;
            auto [it, success]   = a_Renderer->materials.try_emplace(material.get(),
                  Tools::LazyConstructor(
                    [&a_Renderer, &material]() {
                        return std::make_shared<Material>(*a_Renderer, *material);
                    }));
            auto& newMaterial    = it->second;
            materials.push_back(newMaterial);
        }
        pipelineInfo.bindings                   = a_Renderer->defaultShader.GetBindings();
        pipelineInfo.shaderPipelineState.stages = a_Renderer->defaultShader.GetStages();
        graphicsPipelines.push_back(OCRA::Device::CreatePipelineGraphics(a_Renderer->logicalDevice, pipelineInfo));
    }
}
}
