#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/BoundingVolume.hpp>
#include <MSG/Component.hpp>
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Name.hpp>
#include <MSG/Core/Object.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/Transform.hpp>

#include <algorithm>
#include <glm/mat4x4.hpp>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
class MeshSkin : public Component {
public:
    using Joints          = std::vector<ECS::DefaultRegistry::EntityRefType>;
    using InvBindMatrices = std::vector<glm::mat4x4>;
    InvBindMatrices inverseBindMatrices;
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
