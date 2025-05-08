#include <MSG/Renderer/OGL/Subsystems/TransformSubsystem.hpp>

#include <MSG/Renderer/OGL/Renderer.hpp>

#include <MSG/Renderer/OGL/Components/Transform.hpp>

#include <MSG/Mesh.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Transform.hpp>

#include <glm/gtc/matrix_inverse.hpp>

void MSG::Renderer::TransformSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems)
{
    const auto& registry = *a_Renderer.activeScene->GetRegistry();
    // Only get the ones with Mesh since the others won't be displayed
    for (auto& entity : a_Renderer.activeScene->GetVisibleEntities().meshes) {
        if (!registry.HasComponent<Component::Transform>(entity))
            continue;
        auto& sgMesh                      = registry.GetComponent<Mesh>(entity);
        auto& sgTransform                 = registry.GetComponent<MSG::Transform>(entity).GetWorldTransformMatrix();
        auto& rTransform                  = registry.GetComponent<Component::Transform>(entity);
        GLSL::TransformUBO transformUBO   = rTransform.buffer->Get();
        transformUBO.previous             = transformUBO.current;
        transformUBO.current.modelMatrix  = sgMesh.geometryTransform * sgTransform;
        transformUBO.current.normalMatrix = glm::inverseTranspose(transformUBO.current.modelMatrix);
        rTransform.buffer->Set(transformUBO);
        rTransform.buffer->Update();
    }
}