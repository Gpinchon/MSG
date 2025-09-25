#include <MSG/Renderer/OGL/Subsystems/MaterialSubsystem.hpp>

#include <MSG/Renderer/OGL/Renderer.hpp>

#include <MSG/Material.hpp>
#include <MSG/Mesh.hpp>
#include <MSG/Scene.hpp>

#include <unordered_set>

void MSG::Renderer::MaterialSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems)
{
    const auto& registry = *a_Renderer.activeScene->GetRegistry();
    std::unordered_set<std::shared_ptr<MSG::Material>> SGMaterials;
    for (auto& entity : a_Renderer.activeScene->GetVisibleEntities().meshes) {
        auto& sgMesh = registry.GetComponent<Mesh>(entity);
        for (auto& [primitive, material] : sgMesh.at(entity.lod))
            SGMaterials.insert(material);
    }
    for (auto& SGMaterial : SGMaterials) {
        auto material = a_Renderer.materialLoader.Update(a_Renderer, SGMaterial.get());
    }
}