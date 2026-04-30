#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/BoundingVolume.hpp>
#include <MSG/Component.hpp>
#include <MSG/ECS/EntityWeakRef.hpp>
#include <MSG/ECS/Registry.hpp>

#include <glm/fwd.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
/** @brief component used to modify the position of a mesh instance,
 * the mesh transform will be replaced by this entity's transform
 */
class MeshInstanceModifier : public Component {
public:
    /** @brief applies the specified transform to the linked entity,
     * please use this function after modifying the entity's transform
     */
    void ApplyTransform(const glm::mat4x4& a_Transform);
    /** @brief the instance inside the MeshInstances component */
    uint32_t instanceIndex = 0;
    /** @brief the entity the MeshInstances component is attached to */
    ECS::EntityWeakRef<ECS::DefaultRegistry> entityID;
};
}