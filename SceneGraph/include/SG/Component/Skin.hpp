#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Component/Name.hpp>
#include <SG/Core/Buffer/Accessor.hpp>

#include <ECS/Registry.hpp>

#include <vector>
#include <glm/mat4x4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
struct Skin : public Inherit<Object, Skin> {
    using Joints = std::vector<ECS::DefaultRegistry::EntityRefType>;
    using MatrixAccessor = TypedBufferAccessor<glm::mat4>;
    Name name;
    MatrixAccessor inverseBindMatrices;
    Joints joints;
    void AddJoint(const ECS::DefaultRegistry::EntityRefType& joint) {
        joints.push_back(joint);
    }
    void RemoveJoint(const ECS::DefaultRegistry::EntityRefType joint) {
        joints.erase(std::remove(joints.begin(), joints.end(), joint), joints.end());
    }
};
}
