#include <MSG/Renderer/OGL/Subsystems/SkinSubsystem.hpp>

#include <MSG/Renderer/OGL/Renderer.hpp>

#include <MSG/Renderer/OGL/Components/MeshSkin.hpp>

#include <MSG/Mesh/Skin.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Transform.hpp>

void Msg::Renderer::SkinSubsystem::Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (a_Entity.HasComponent<Msg::MeshSkin>() && !a_Entity.HasComponent<Renderer::MeshSkin>()) {
        auto& sgMeshSkin = a_Entity.GetComponent<Msg::MeshSkin>();
        auto parent      = a_Entity.GetComponent<Msg::Parent>().Lock();
        auto& transform  = parent.GetComponent<Msg::Transform>().GetWorldTransformMatrix();
        a_Entity.AddComponent<Renderer::MeshSkin>(a_Renderer.context, transform, sgMeshSkin);
    }
}

void Msg::Renderer::SkinSubsystem::Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (a_Entity.HasComponent<Renderer::MeshSkin>())
        a_Entity.RemoveComponent<Renderer::MeshSkin>();
}

void Msg::Renderer::SkinSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems)
{
    auto& registry = *a_Renderer.activeScene->GetRegistry();
    for (auto& entity : a_Renderer.activeScene->GetVisibleEntities().skins) {
        auto& sgTransform = registry.GetComponent<Msg::Transform>(entity).GetWorldTransformMatrix();
        auto& sgMeshSkin  = registry.GetComponent<Msg::MeshSkin>(entity);
        auto& rMeshSkin   = registry.GetComponent<Renderer::MeshSkin>(entity);
        rMeshSkin.Update(a_Renderer.context, sgTransform, sgMeshSkin);
    }
}