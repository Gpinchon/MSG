#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Buffer/Accessor.hpp>
#include <MSG/BoundingVolume.hpp>
#include <MSG/Core/Name.hpp>
#include <MSG/Transform.hpp>
#include <MSG/ECS/Registry.hpp>

#include <algorithm>
#include <glm/mat4x4.hpp>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class MeshSkin : public Core::Inherit<Core::Object, MeshSkin> {
public:
    using Joints         = std::vector<ECS::DefaultRegistry::EntityRefType>;
    using MatrixAccessor = BufferTypedAccessor<glm::mat4>;
    Core::Name name;
    MatrixAccessor inverseBindMatrices;
    Joints joints;
    float jointsRadius = 0.1f; // the radius of joints used for BV calculation
    void AddJoint(const ECS::DefaultRegistry::EntityRefType& joint)
    {
        joints.push_back(joint);
    }
    void RemoveJoint(const ECS::DefaultRegistry::EntityRefType joint)
    {
        joints.erase(std::remove(joints.begin(), joints.end(), joint), joints.end());
    }
    BoundingVolume ComputeBoundingVolume() const
    {
        BoundingVolume boundingVolume;
        for (auto& joint : joints) {
            auto& jointTr  = joint.GetComponent<Transform>();
            auto& jointPos = jointTr.GetWorldPosition();
            boundingVolume += Sphere(jointPos, jointsRadius);
        }
        return boundingVolume;
    }
};
}
