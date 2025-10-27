#include <MSG/Renderer/OGL/Subsystems/MaterialSubsystem.hpp>

#include <MSG/Renderer/OGL/Renderer.hpp>

#include <MSG/Material.hpp>
#include <MSG/MaterialSet.hpp>
#include <MSG/Scene.hpp>

#include <unordered_set>

void Msg::Renderer::MaterialSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems)
{
    const auto& registry = *a_Renderer.activeScene->GetRegistry();
    std::unordered_set<std::shared_ptr<Msg::Material>> SGMaterials;
    for (auto& entity : a_Renderer.activeScene->GetVisibleEntities().entities) {
        if (!registry.HasComponent<MaterialSet>(entity))
            continue;
        auto& sgMaterials = registry.GetComponent<MaterialSet>(entity);
        for (auto& material : sgMaterials.materials)
            SGMaterials.insert(material);
    }
    for (auto& SGMaterial : SGMaterials) {
        auto material = a_Renderer.materialLoader.Update(a_Renderer, SGMaterial.get());
    }
}