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
#include <SG/Component/Children.hpp>
#include <SG/Component/Name.hpp>
#include <SG/Component/Parent.hpp>
#include <SG/Component/Transform.hpp>
#include <SG/Entity/Entity.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

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
template <typename RegistryType>
auto Create(const RegistryType& a_Registry)
{
    auto entity                                     = SG::Entity::Create(a_Registry);
    entity.template GetComponent<Component::Name>() = "Node_" + std::to_string(++GetNbr());
    entity.template AddComponent<Component::Transform>();
    entity.template AddComponent<Component::Parent>();
    return entity;
}

/**
 * @brief sets a_Parent to be parent of a_Child and adds a_Child in parent's Children
 * @param a_Child : the Node whose parent is to be set
 * @param a_Parent : a_Child's new parent
 */
template <typename EntityRefType>
auto SetParent(const EntityRefType& a_Child, const EntityRefType& a_Parent)
{
    auto& parent   = a_Child.template GetComponent<Component::Parent>();
    auto& children = a_Parent.template GetComponent<Component::Children>();
    parent         = typename EntityRefType::IDType(a_Parent);
    children.insert(a_Child);
}

/**
 * @brief removes the parent of a_Child and removes a_Child from a_Parent's Children
 * @param a_Child : the Node whose parent is to be removed
 * @param a_Parent : a_Child's current parent
 */
template <typename EntityRefType>
auto RemoveParent(const EntityRefType& a_Child, const EntityRefType& a_Parent)
{
    a_Child.template GetComponent<Component::Parent>().reset();
    a_Parent.template GetComponent<Component::Children>().erase(a_Child);
}

template <typename EntityRefType>
glm::mat4 GetWorldTransformMatrix(const EntityRefType& a_Node)
{
    const auto& transform      = a_Node.template GetComponent<Component::Transform>();
    const auto transformMatrix = transform.translationMatrix * transform.rotationMatrix * transform.scaleMatrix;
    if (!a_Node.template HasComponent<Component::Parent>())
        return transformMatrix;
    const auto& parent = a_Node.template GetComponent<Component::Parent>();
    if (parent) {
        const auto parentEntity = a_Node.GetRegistry()->GetEntityRef(parent);
        return GetWorldTransformMatrix(parentEntity) * transformMatrix;
    }
    return transformMatrix;
}

template <typename EntityRefType>
glm::mat4 GetWorldTranslationMatrix(const EntityRefType& a_Node)
{
    const auto& translationMatrix = a_Node.template GetComponent<Component::Transform>().translationMatrix;
    if (!a_Node.template HasComponent<Component::Parent>())
        return translationMatrix;
    const auto& parent = a_Node.template GetComponent<Component::Parent>();
    if (parent) {
        auto parentEntity = a_Node.GetRegistry()->GetEntityRef(parent);
        return GetWorldTranslationMatrix(parentEntity) * translationMatrix;
    }
    return translationMatrix;
}

template <typename EntityRefType>
glm::mat4 GetWorldRotationMatrix(const EntityRefType& a_Node)
{
    const auto& rotationMatrix = a_Node.template GetComponent<Component::Transform>().rotationMatrix;
    if (!a_Node.template HasComponent<Component::Parent>())
        return rotationMatrix;
    const auto& parent = a_Node.template GetComponent<Component::Parent>();
    if (parent) {
        auto parentEntity = a_Node.GetRegistry()->GetEntityRef(parent);
        return GetWorldRotationMatrix(parentEntity) * rotationMatrix;
    }
    return rotationMatrix;
}

template <typename EntityRefType>
glm::mat4 GetWorldScaleMatrix(const EntityRefType& a_Node)
{
    const auto& scaleMatrix = a_Node.template GetComponent<Component::Transform>().scaleMatrix;
    if (!a_Node.template HasComponent<Component::Parent>())
        return scaleMatrix;
    const auto& parent = a_Node.template GetComponent<Component::Parent>();
    if (parent) {
        auto parentEntity = a_Node.GetRegistry()->GetEntityRef(parent);
        return GetWorldScaleMatrix(parentEntity) * scaleMatrix;
    }
    return scaleMatrix;
}

template <typename EntityRefType>
glm::vec3 GetWorldPosition(const EntityRefType& a_Node)
{
    const auto& position = a_Node.template GetComponent<Component::Transform>().position;
    if (!a_Node.template HasComponent<Component::Parent>())
        return position;
    const auto& parent = a_Node.template GetComponent<Component::Parent>();
    if (parent) {
        auto parentEntity = a_Node.GetRegistry()->GetEntityRef(parent);
        return GetWorldTransformMatrix(parentEntity) * glm::vec4(position, 1);
    }
    return position;
}

template <typename EntityRefType>
glm::quat GetWorldRotation(const EntityRefType& a_Node)
{
    const auto& rotation = a_Node.template GetComponent<Component::Transform>().rotation;
    if (!a_Node.template HasComponent<Component::Parent>())
        return rotation;
    const auto& parent = a_Node.template GetComponent<Component::Parent>();
    if (parent) {
        auto parentEntity = a_Node.GetRegistry()->GetEntityRef(parent);
        return GetWorldTransformMatrix(parentEntity) * glm::mat4_cast(rotation);
    }
    return glm::mat4(1.f) * glm::mat4_cast(rotation);
}

template <typename EntityRefType>
glm::vec3 GetWorldScale(const EntityRefType& a_Node)
{
    const auto& scale = a_Node.template GetComponent<Component::Transform>().scale;
    if (!a_Node.template HasComponent<Component::Parent>())
        return scale;
    const auto& parent = a_Node.template GetComponent<Component::Parent>();
    if (parent) {
        auto parentEntity = a_Node.GetRegistry()->GetEntityRef(parent);
        return GetWorldTransformMatrix(parentEntity) * glm::vec4(scale, 1);
    }
    return glm::mat4(1.f) * glm::vec4(scale, 1);
}

template <typename EntityRefType>
auto GetForward(const EntityRefType& a_Node)
{
    return GetWorldRotation(a_Node) * a_Node.template GetComponent<Component::Transform>().forward;
}

template <typename EntityRefType>
auto GetRight(const EntityRefType& a_Node)
{
    return GetWorldRotation(a_Node) * a_Node.template GetComponent<Component::Transform>().right;
}

template <typename EntityRefType>
auto GetUp(const EntityRefType& a_Node)
{
    return GetWorldRotation(a_Node) * a_Node.template GetComponent<Component::Transform>().up;
}

template <typename EntityRefType>
auto LookAt(const EntityRefType& a_Node, const glm::vec3& a_Target)
{
    auto direction  = glm::normalize(a_Target - GetWorldPosition(a_Node));
    auto directionL = glm::length(direction);
    auto up         = GetUp(a_Node);
    auto& transform = a_Node.template GetComponent<Component::Transform>();
    if (!(directionL > 0.0001)) {
        transform.SetRotation(glm::quat(1, 0, 0, 0));
        return;
    }
    direction /= directionL;
    if (glm::abs(glm::dot(direction, up)) > 0.9999f) {
        up = glm::vec3(1, 0, 0);
    }
    transform.SetRotation(glm::quatLookAt(direction, up));
}

template <typename EntityRefType>
auto LookAt(const EntityRefType& a_Node, const EntityRefType& a_Target)
{
    return LookAt(a_Node, GetWorldPosition(a_Target));
}

}
