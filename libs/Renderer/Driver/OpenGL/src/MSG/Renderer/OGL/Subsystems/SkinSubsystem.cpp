#include <MSG/Renderer/OGL/Subsystems/SkinSubsystem.hpp>

#include <MSG/Renderer/OGL/Renderer.hpp>

#include <MSG/Renderer/OGL/Components/MeshSkin.hpp>

#include <MSG/Mesh/Skin.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Transform.hpp>

void Msg::Renderer::SkinSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems)
{
    auto& registry = *a_Renderer.activeScene->GetRegistry();
    for (auto& entity : a_Renderer.activeScene->GetVisibleEntities().skins) {
        auto& sgTransform = registry.GetComponent<Msg::Transform>(entity).GetWorldTransformMatrix();
        auto& sgMeshSkin  = registry.GetComponent<MeshSkin>(entity);
        auto& rMeshSkin   = registry.GetComponent<Component::MeshSkin>(entity);
        rMeshSkin.Update(a_Renderer.context, sgTransform, sgMeshSkin);
    }
}