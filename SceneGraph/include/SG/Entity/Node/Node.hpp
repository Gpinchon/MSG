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
#include <SG/Entity/Entity.hpp>
#include <SG/Component/Name.hpp>
#include <SG/Component/Transform.hpp>
#include <SG/Component/Parent.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Node {
#define NODE_COMPONENTS ENTITY_COMPONENTS, SG::Component::Transform, SG::Component::Parent
/** @return the total nbr of Nodes created since start-up */
uint32_t& GetNbr();
template<typename RegistryType>
auto Create(const RegistryType& a_Registry) {
    auto entity = SG::Entity::Create(a_Registry);
    entity.GetComponent<Component::Name>() = "Node_" + std::to_string(++GetNbr());
    entity.AddComponent<Component::Transform>();
    entity.AddComponent<Component::Parent>();
    return entity;
}

/**
 * @brief sets a_Parent to be parent of a_Child and adds a_Child in parent's Children
 * @param a_Child : the Node whose parent is to be set
 * @param a_Parent : a_Child's new parent
*/
template<typename EntityRefType>
auto SetParent(const EntityRefType& a_Child, const EntityRefType& a_Parent) {
    auto& parent = a_Child.GetComponent<Component::Parent>();
    auto& children = a_Parent.GetComponent<Component::Children>();
    parent = EntityRefType::IDType(a_Parent);
    children.insert(a_Child);
}

/**
 * @brief removes the parent of a_Child and removes a_Child from a_Parent's Children
 * @param a_Child : the Node whose parent is to be removed
 * @param a_Parent : a_Child's current parent
*/
template<typename EntityRefType>
auto RemoveParent(const EntityRefType& a_Child, const EntityRefType& a_Parent) {
    a_Child.GetComponent<Component::Parent>().reset();
    a_Parent.GetComponent<Component::Children>().erase(a_Child);
}

template<typename EntityRefType>
glm::mat4 GetWorldTransformMatrix(const EntityRefType& a_Node) {
    const auto& parent = a_Node.GetComponent<Component::Parent>();
    const auto& transform = a_Node.GetComponent<Component::Transform>();
    const auto localTransformMatrix = transform.GetLocalTranslationMatrix() * transform.GetLocalRotationMatrix() * transform.GetLocalScaleMatrix();
    if (parent) {
        const auto parentEntity = a_Node.GetRegistry()->GetEntityRef(parent);
        return GetWorldTransformMatrix(parentEntity) * localTransformMatrix;
    }
    return localTransformMatrix;
}

template<typename EntityRefType>
glm::mat4 GetWorldTranslationMatrix(const EntityRefType& a_Node) {
    const auto& parent = a_Node.GetComponent<Component::Parent>();
    const auto& localTransformMatrix = a_Node.GetComponent<Component::Transform>().GetLocalTranslationMatrix();
    if (parent) {
        auto parentEntity = a_Node.GetRegistry()->GetEntityRef(parent);
        return GetWorldTranslationMatrix(parentEntity) * localTransformMatrix;
    }
    return localTransformMatrix;
}

template<typename EntityRefType>
glm::mat4 GetWorldRotationMatrix(const EntityRefType& a_Node) {
    const auto& parent = a_Node.GetComponent<Component::Parent>();
    const auto& localTransformMatrix = a_Node.GetComponent<Component::Transform>().GetLocalRotationMatrix();
    if (parent) {
        auto parentEntity = a_Node.GetRegistry()->GetEntityRef(parent);
        return GetWorldRotationMatrix(parentEntity) * localTransformMatrix;
    }
    return localTransformMatrix;
}

template<typename EntityRefType>
glm::mat4 GetWorldScaleMatrix(const EntityRefType& a_Node) {
    const auto& parent = a_Node.GetComponent<Component::Parent>();
    const auto& localTransformMatrix = a_Node.GetComponent<Component::Transform>().GetLocalScaleMatrix();
    if (parent) {
        auto parentEntity = a_Node.GetRegistry()->GetEntityRef(parent);
        return GetWorldScaleMatrix(parentEntity) * localTransformMatrix;
    }
    return localTransformMatrix;
}

template<typename EntityRefType>
glm::vec3 GetWorldPosition(const EntityRefType& a_Node)
{
    const auto& parent = a_Node.GetComponent<Component::Parent>();
    const auto& localPosition = a_Node.GetComponent<Component::Transform>().GetPosition();
    return (parent ? GetWorldTransformMatrix(parent) : glm::mat4(1.f)) * glm::vec4(localPosition, 1);
}

template<typename EntityRefType>
glm::quat GetWorldRotation(const EntityRefType& a_Node)
{
    const auto& parent = a_Node.GetComponent<Component::Parent>();
    const auto& localRotation = a_Node.GetComponent<Component::Transform>().GetRotation();
    return (parent ? GetWorldTransformMatrix(parent) : glm::mat4(1.f)) * glm::mat4_cast(localRotation);
}

template<typename EntityRefType>
glm::vec3 GetWorldScale(const EntityRefType& a_Node)
{
    const auto& parent = a_Node.GetComponent<Component::Parent>();
    const auto& localScale = a_Node.GetComponent<Component::Transform>().GetScale();
    return (parent ? GetWorldTransformMatrix(parent) : glm::mat4(1.f)) * glm::vec4(localScale, 1);
}

template<typename EntityRefType>
auto GetForward(const EntityRefType& a_Entity) {
    return NodeGetWorldRotation() * a_Entity.GetComponent<Component::Transform>().GetForward();
}

template<typename EntityRefType>
auto GetRight(const EntityRefType& a_Entity) {
    return NodeGetWorldRotation() * a_Entity.GetComponent<Component::Transform>().GetRight();
}

template<typename EntityRefType>
auto GetUp(const EntityRefType& a_Entity) {
    return NodeGetWorldRotation() * a_Entity.GetComponent<Component::Transform>().GetUp();
}
}
