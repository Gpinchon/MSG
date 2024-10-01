#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Component/BoundingVolume.hpp>
#include <SG/Component/Name.hpp>
#include <SG/Component/Transform.hpp>
#include <SG/Core/Buffer/Accessor.hpp>

#include <ECS/Registry.hpp>

#include <algorithm>
#include <glm/mat4x4.hpp>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
class MeshSkin : public Inherit<Object, MeshSkin> {
public:
    using Joints         = std::vector<ECS::DefaultRegistry::EntityRefType>;
    using MatrixAccessor = TypedBufferAccessor<glm::mat4>;
    Name name;
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
            auto& jointTr  = joint.GetComponent<SG::Component::Transform>();
            auto& jointPos = jointTr.GetWorldPosition();
            boundingVolume += BoundingSphere(jointPos, jointsRadius);
        }
        return boundingVolume;
    }
};
}
