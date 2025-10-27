#include <MSG/Renderer/OGL/Subsystems/MaterialSubsystem.hpp>

#include <MSG/Renderer/OGL/Material.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>

#include <MSG/Material.hpp>
#include <MSG/MaterialSet.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Tools/LazyConstructor.hpp>

#include <unordered_set>

Msg::Renderer::MaterialSubsystem::~MaterialSubsystem()
{
    MSGCheckErrorFatal(!materialCache.empty(), "Not all materials were unloaded !");
}

void Msg::Renderer::MaterialSubsystem::Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (a_Entity.HasComponent<Msg::MaterialSet>() && !a_Entity.HasComponent<Renderer::MaterialSet>()) {
        auto& rMaterials  = a_Entity.AddComponent<Renderer::MaterialSet>();
        auto& sgMaterials = a_Entity.GetComponent<Msg::MaterialSet>();
        rMaterials.fill(nullptr);
        for (uint8_t i = 0; i < MSG_MAX_MATERIALS; i++) {
            auto& sgMaterial = sgMaterials[i];
            if (sgMaterial == nullptr)
                continue;
            Tools::LazyConstructor factory = [this, &a_Renderer, &sgMaterial] {
                auto material = std::make_shared<Material>(a_Renderer.context);
                material->Set(a_Renderer, *sgMaterial);
                return std::move(material);
            };
            rMaterials[i] = materialCache.GetOrCreate(sgMaterial.get(), factory);
        }
    }
}

void Msg::Renderer::MaterialSubsystem::Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (a_Entity.HasComponent<Renderer::MaterialSet>()) {
        a_Entity.RemoveComponent<Renderer::MaterialSet>();
        auto& sgMaterials = a_Entity.GetComponent<Msg::MaterialSet>();
        for (auto& sgMtl : sgMaterials) {
            if (sgMtl == nullptr)
                continue;
            auto itr = materialCache.find(sgMtl.get());
            if (itr != materialCache.end() && itr->second.use_count() == 1)
                materialCache.erase(itr);
        }
    }
}

void Msg::Renderer::MaterialSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems)
{
    auto& registry = *a_Renderer.activeScene->GetRegistry();
    std::unordered_set<std::shared_ptr<Msg::Material>> sgMaterialSet;
    for (auto& entity : a_Renderer.activeScene->GetVisibleEntities().entities) {
        if (!registry.HasComponent<MaterialSet>(entity))
            continue;
        auto& sgMaterials = registry.GetComponent<Msg::MaterialSet>(entity);
        for (auto& material : sgMaterials.materials) {
            if (material != nullptr)
                sgMaterialSet.insert(material);
        }
    }
    for (auto& sgMaterial : sgMaterialSet) {
        auto materialItr = materialCache.find(sgMaterial.get());
        if (materialItr != materialCache.end()) {
            auto& material = materialItr->second;
            material->Set(a_Renderer, *sgMaterial);
        } else {
            MSGErrorWarning("Material not found, loading new material");
            Tools::LazyConstructor factory = [this, &a_Renderer, &sgMaterial] {
                auto material = std::make_shared<Material>(a_Renderer.context);
                material->Set(a_Renderer, *sgMaterial);
                return std::move(material);
            };
            materialCache.GetOrCreate(sgMaterial.get(), factory);
        }
    }
}