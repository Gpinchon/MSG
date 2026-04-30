#include <MSG/MeshInstanceModifier.hpp>
#include <MSG/MeshInstances.hpp>

void Msg::MeshInstanceModifier::ApplyTransform(const glm::mat4x4& a_Transform)
{
    assert(!entityID.Expired() && "Entity reference expired !");
    auto entityRef                          = entityID.Lock();
    auto& meshInstances                     = entityRef.GetComponent<MeshInstances>();
    meshInstances.transforms[instanceIndex] = a_Transform;
}