/*
* @Author: gpinchon
* @Date:   2021-06-19 15:01:38
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-26 23:39:40
*/

#include <SG/Node/Node.hpp>
#include <SG/Node/NodeGroup.hpp>

#include <glm/gtx/transform.hpp>

namespace TabGraph::SG {
Node::Node()
    : Inherit()
{
    static auto s_nodeNbr { 0u };
    SetName("Node_" + std::to_string(++s_nodeNbr));
}
Node::Node(const Node& other)
    : Node()
{
    _position = other._position;
    _rotation = other._rotation;
    _scale = other._scale;
    _positionChanged = true;
    _rotationChanged = true;
    _scaleChanged = true;
}
glm::mat4 Node::GetLocalTransformMatrix() {
    if (_positionChanged || _rotationChanged || _scaleChanged)
        _localTransformMatrix = GetLocalTranslationMatrix() * GetLocalRotationMatrix() * GetLocalScaleMatrix();
    return _localTransformMatrix;
}
glm::mat4 Node::GetLocalTranslationMatrix()
{
    if (_positionChanged) {
        _localTranslationMatrix = glm::translate(GetLocalPosition());
        _positionChanged = false;
    }
    return _localTranslationMatrix;
}
glm::mat4 Node::GetLocalRotationMatrix()
{
    if (_rotationChanged) {
        _localRotationMatrix = glm::mat4_cast(GetLocalRotation());
        _rotationChanged = false;
    }
    return _localRotationMatrix;
}
glm::mat4 Node::GetLocalScaleMatrix()
{
    if (_scaleChanged) {
        _localScaleMatrix = glm::scale(GetLocalScale());
        _scaleChanged = false;
    }
    return _localScaleMatrix;
}

void Node::SetParent(std::shared_ptr<NodeGroup> parent)
{
    if (GetParent() != nullptr)
        GetParent()->_RemoveChild(shared_from_this());
    _parent = parent;
    if (parent != nullptr)
        parent->_AddChild(shared_from_this());
}
glm::mat4 Node::GetWorldTransformMatrix()
{
    return (GetParent() ? GetParent()->GetWorldTransformMatrix() : glm::mat4(1.f)) * GetLocalTransformMatrix();
}

glm::mat4 Node::GetWorldTranslationMatrix()
{
    return (GetParent() ? GetParent()->GetWorldTransformMatrix() : glm::mat4(1.f)) * GetLocalTranslationMatrix();
}

glm::mat4 Node::GetWorldRotationMatrix()
{
    return (GetParent() ? GetParent()->GetWorldTransformMatrix() : glm::mat4(1.f)) * GetLocalRotationMatrix();
}

glm::mat4 Node::GetWorldScaleMatrix()
{
    return (GetParent() ? GetParent()->GetWorldTransformMatrix() : glm::mat4(1.f)) * GetLocalScaleMatrix();
}
std::ostream& Node::Serialize(std::ostream& a_Ostream) const {
    Object::Serialize(a_Ostream);
    if (!_parent.expired()) SerializeProperty(a_Ostream, "Parent", _parent.lock()->GetId());
    return a_Ostream;
}
glm::vec3 Node::GetWorldPosition() const
{
    return (GetParent() ? GetParent()->GetWorldTransformMatrix() : glm::mat4(1.f)) * glm::vec4(GetLocalPosition(), 1);
}
glm::quat Node::GetWorldRotation() const
{
    return (GetParent() ? GetParent()->GetWorldTransformMatrix() : glm::mat4(1.f)) * glm::mat4_cast(GetLocalRotation());
}
glm::vec3 Node::GetWorldScale() const
{
    return (GetParent() ? GetParent()->GetWorldTransformMatrix() : glm::mat4(1.f)) * glm::vec4(GetLocalScale(), 1);
}
}
