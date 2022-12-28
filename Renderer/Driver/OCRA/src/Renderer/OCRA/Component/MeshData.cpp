#include <Renderer/OCRA/Component/MeshData.hpp>

#include <Renderer/OCRA/Renderer.hpp>
#include <Renderer/OCRA/Primitive.hpp>
#include <Renderer/OCRA/Material.hpp>

#include <SG/Component/Mesh.hpp>

#include <Tools/LazyConstructor.hpp>

namespace TabGraph::Renderer::Component {
MeshData::MeshData(const Renderer::Handle& a_Renderer, const SG::Component::Mesh& a_Mesh)
{
    for (const auto& it : a_Mesh.primitives) {
        {
            const auto& primitive = it.first;
            auto [it, success] = a_Renderer->primitives.try_emplace(primitive.get(), Tools::LazyConstructor([=] {
                return std::make_shared<Primitive>(a_Renderer->physicalDevice, a_Renderer->logicalDevice, *primitive);
            }));
            primitives.push_back(it->second);
        }
        {
            const auto& material = it.second;
            auto [it, success] = a_Renderer->materials.try_emplace(material.get(), Tools::LazyConstructor([=] {
                return std::make_shared<Material>(a_Renderer->physicalDevice, a_Renderer->logicalDevice, *material);
            }));
            materials.push_back(it->second);
        }
    }
}
}