#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/BoundingVolume.hpp>
#include <MSG/Component.hpp>

#include <glm/mat4x4.hpp>

#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
class Mesh;
class MeshSkin;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
/** @brief component used to instantiate a mesh component attached to the entity */
class MeshInstances : public Component {
public:
    /** @brief calculates the bounding volume of all the mesh instances */
    void ComputeBoundingVolume(const Mesh* a_Mesh, const MeshSkin* a_MeshSkin);
    /** @brief if true, lod won't be compoted on each instance and globalLod will be used instead */
    bool useGlobalLod = true;
    /** @brief the global lod to apply to the mesh instances */
    uint8_t globalLod = 0;
    /** @brief the number of instances */
    uint32_t instances = 1;
    /** @brief transforms used to offset the meshes */
    std::vector<glm::mat4x4> transforms;
    BoundingVolume boundingVolume;
};
}