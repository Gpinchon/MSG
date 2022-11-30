/*
* @Author: gpinchon
* @Date:   2021-06-19 14:57:45
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-04 16:38:23
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Common.hpp>
#include <SG/Core/Inherit.hpp>

#include <SG/Core/Object.hpp>
#include <SG/Node/Transform.hpp>
#include <SG/Node/Parent.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>

#include <memory>
#include <set>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class NodeGroup;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
#define NODE_COMPONENTS OBJECT_COMPONENTS, SG::Transform, SG::Parent
/** @return the total nbr of Nodes created since start-up */
uint32_t& GetNodeNbr();
template<typename RegistryType>
auto CreateNode(const RegistryType& a_Registry) {
    auto entity = SG::CreateObject(a_Registry);
    entity.GetComponent<SG::Name>() = "Node_" + std::to_string(++GetNodeNbr());
    entity.AddComponent<SG::Transform>();
    entity.AddComponent<SG::Parent>();
    return entity;
}

template<typename EntityRefType>
auto NodeSetParent(const EntityRefType& a_Child, const EntityRefType& a_Parent) {
    auto& parent = a_Child.GetComponent<SG::Parent>();
    auto& children = a_Parent.GetComponent<SG::Children>();
    parent = a_Parent;
    children.insert(a_Child);
}

template<typename EntityRefType>
auto NodeRemoveParent(const EntityRefType& a_Child, const EntityRefType& a_Parent) {
    a_Child.GetComponent<SG::Parent>().reset();
    a_Parent.GetComponent<SG::Children>().erase(a_Child);
}

template<typename EntityRefType>
glm::mat4 NodeGetWorldTransformMatrix(const EntityRefType& a_Node) {
    const auto& parent = a_Node.GetComponent<SG::Parent>();
    const auto& transform = a_Node.GetComponent<SG::Transform>();
    const auto localTransformMatrix = transform.localTranslationMatrix * transform.localRotationMatrix * transform.localScaleMatrix;
    if (parent) {
        const auto parentEntity = a_Node.GetRegistry()->GetEntityRef(parent);
        return NodeGetWorldTransformMatrix(parentEntity) * localTransformMatrix;
    }
    return localTransformMatrix;
}

template<typename EntityRefType>
glm::mat4 NodeGetWorldTranslationMatrix(const EntityRefType& a_Node) {
    const auto& parent = a_Node.GetComponent<SG::Parent>();
    const auto& localTransformMatrix = a_Node.GetComponent<SG::Transform>().localTranslationMatrix;
    if (parent) {
        auto parentEntity = a_Node.GetRegistry()->GetEntityRef(parent);
        return NodeGetWorldTranslationMatrix(parentEntity) * localTransformMatrix;
    }
    return localTransformMatrix;
}

template<typename EntityRefType>
glm::mat4 NodeGetWorldRotationMatrix(const EntityRefType& a_Node) {
    const auto& parent = a_Node.GetComponent<SG::Parent>();
    const auto& localTransformMatrix = a_Node.GetComponent<SG::Transform>().localRotationMatrix;
    if (parent) {
        auto parentEntity = a_Node.GetRegistry()->GetEntityRef(parent);
        return NodeGetWorldRotationMatrix(parentEntity) * localTransformMatrix;
    }
    return localTransformMatrix;
}

template<typename EntityRefType>
glm::mat4 NodeGetWorldScaleMatrix(const EntityRefType& a_Node) {
    const auto& parent = a_Node.GetComponent<SG::Parent>();
    const auto& localTransformMatrix = a_Node.GetComponent<SG::Transform>().localScaleMatrix;
    if (parent) {
        auto parentEntity = a_Node.GetRegistry()->GetEntityRef(parent);
        return NodeGetWorldScaleMatrix(parentEntity) * localTransformMatrix;
    }
    return localTransformMatrix;
}

template<typename EntityRefType>
glm::vec3 NodeGetWorldPosition(const EntityRefType& a_Node)
{
    const auto& parent = a_Node.GetComponent<SG::Parent>();
    const auto& localPosition = a_Node.GetComponent<SG::Transform>().position;
    return (parent ? NodeGetWorldTransformMatrix(parent) : glm::mat4(1.f)) * glm::vec4(localPosition, 1);
}

template<typename EntityRefType>
glm::quat NodeGetWorldRotation(const EntityRefType& a_Node)
{
    const auto& parent = a_Node.GetComponent<SG::Parent>();
    const auto& localRotation = a_Node.GetComponent<SG::Transform>().rotation;
    return (parent ? NodeGetWorldTransformMatrix(parent) : glm::mat4(1.f)) * glm::mat4_cast(localRotation);
}

template<typename EntityRefType>
glm::vec3 NodeGetWorldScale(const EntityRefType& a_Node)
{
    const auto& parent = a_Node.GetComponent<SG::Parent>();
    const auto& localScale = a_Node.GetComponent<SG::Transform>().scale;
    return (parent ? NodeGetWorldTransformMatrix(parent) : glm::mat4(1.f)) * glm::vec4(localScale, 1);
}

/**
 * @brief Describes a leaf of the SceneGraph, can only have parent
*/
class Node : public Inherit<Object, Node>, public std::enable_shared_from_this<Node> {
public:
    Node();
    Node(const Node& a_Node);
    Node(const std::string& a_Name)
        : Node()
    {
        SetName(a_Name);
    }
    /** @return the Node's parent */
    auto GetParent() const
    {
        return _parent.lock();
    }
    /**
     * @brief sets the parent to parent and calls AddChild in parent
     * @param parent : the Node's new parent
    */
    void SetParent(std::shared_ptr<NodeGroup> a_Parent);

    inline auto GetLocalPosition() const
    {
        return _position;
    }
    inline auto GetLocalScale() const
    {
        return _scale;
    }
    inline auto GetLocalRotation() const
    {
        return _rotation;
    }

    glm::vec3 GetWorldPosition() const;
    glm::quat GetWorldRotation() const;
    glm::vec3 GetWorldScale() const;

    inline void SetLocalPosition(const glm::vec3& a_Position)
    {
        _positionChanged |= a_Position != GetLocalPosition();
        _position = a_Position;
    }
    inline void SetLocalScale(const glm::vec3& a_Scale)
    {
        _scaleChanged |= a_Scale != GetLocalScale();
        _scale = a_Scale;
    }
    inline void SetLocalRotation(const glm::quat& a_Rotation)
    {
        _rotationChanged |= a_Rotation != GetLocalRotation();
        _rotation = a_Rotation;
    }

    glm::mat4 GetLocalTransformMatrix();
    glm::mat4 GetLocalTranslationMatrix();
    glm::mat4 GetLocalRotationMatrix();
    glm::mat4 GetLocalScaleMatrix();

    glm::mat4 GetWorldTransformMatrix();
    glm::mat4 GetWorldTranslationMatrix();
    glm::mat4 GetWorldRotationMatrix();
    glm::mat4 GetWorldScaleMatrix();

    /**
     * @brief READONLY : Computed on demand
     * @return a_SceneForward * Rotation()
     */
    glm::vec3 Forward(const glm::vec3& a_SceneForward = Common::Forward()) const {
        return a_SceneForward * GetWorldRotation();
    }
    /**
     * @brief READONLY : Computed on demand
     * @return a_SceneUp * Rotation()
     */
    glm::vec3 Up(const glm::vec3& a_SceneUp = Common::Up()) const {
        return a_SceneUp * GetWorldRotation();
    }
    /**
     * @brief READONLY : Computed on demand
     * a_SceneRight * Rotation()
     */
    glm::vec3 Right(const glm::vec3& a_SceneRight = Common::Right()) const {
        return a_SceneRight * GetWorldRotation();
    }

    void LookAt(const glm::vec3& a_Target, const glm::vec3& a_Up = Common::Up());
    void LookAt(const std::shared_ptr<Node>& a_Target, const glm::vec3& a_Up = Common::Up());

    /**
     * @brief calls operator()
     * @param visitor : a Visitor to visit Node
    */
    inline virtual void Accept(Visitor& visitor) {
        return visitor(*this);
    }

private:
    bool _positionChanged { true };
    bool _rotationChanged { true };
    bool _scaleChanged { true };

    glm::vec3 _position { 0 };
    glm::vec3 _scale { 1 };
    glm::quat _rotation { glm::vec3(0.0, 0.0, 0.0) };

    glm::mat4 _localTransformMatrix { glm::mat4(1) };
    glm::mat4 _localTranslationMatrix {};
    glm::mat4 _localRotationMatrix {};
    glm::mat4 _localScaleMatrix {};
    std::weak_ptr<NodeGroup> _parent;
};
}
