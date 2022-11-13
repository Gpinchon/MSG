#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Object.hpp>
#include <Core/Inherit.hpp>
#include <Shapes/Mesh/Mesh.hpp>
#include <Buffer/Accessor.hpp>

#include <vector>
#include <glm/mat4x4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Nodes {
class Node;
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
class Mesh::Skin : public Core::Inherit<Core::Object, Mesh::Skin> {
public:
    Skin();
    auto& GetInverseBindMatrices() const
    {
        return _inverseBindMatrices;
    }
    void SetInverseBindMatrices(const Buffer::TypedAccessor<glm::mat4>& inverseBindMatrices)
    {
        _inverseBindMatrices = inverseBindMatrices;
    }
    auto& Joints() const
    {
        return _joints;
    }
    void AddJoint(std::shared_ptr<Nodes::Node> joint)
    {
        _joints.push_back(joint);
    }
    void RemoveJoint(std::shared_ptr<Nodes::Node> joint)
    {
        std::remove(_joints.begin(), _joints.end(), joint);
    }

private:
    std::vector<std::shared_ptr<Nodes::Node>> _joints;
    Buffer::TypedAccessor<glm::mat4> _inverseBindMatrices;
};
}
