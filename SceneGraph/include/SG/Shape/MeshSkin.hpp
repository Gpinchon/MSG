#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Object.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Shape/Mesh.hpp>
#include <SG/Buffer/Accessor.hpp>

#include <vector>
#include <glm/mat4x4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Node;
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Mesh::Skin : public Inherit<Object, Mesh::Skin> {
    using Joints = std::vector<std::shared_ptr<Node>>;
    using MatrixAccessor = TypedBufferAccessor<glm::mat4>;
    PROPERTY(MatrixAccessor, InverseBindMatrices, );
    PROPERTY(Joints, Joints, );

public:
    Skin() = default;
    void AddJoint(std::shared_ptr<Node> joint) {
        GetJoints().push_back(joint);
    }
    void RemoveJoint(std::shared_ptr<Node> joint) {
        GetJoints().erase(std::remove(GetJoints().begin(), GetJoints().end(), joint), GetJoints().end());
    }
};
}
