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
#define NODE_COMPONENTS ENTITY_COMPONENTS, SG::Component::Transform, SG::Component::WorldTransform, SG::Component::Parent
/** @return the total nbr of Nodes created since start-up */
uint32_t& GetNbr();
template <typename RegistryType>
auto Create(const RegistryType& a_Registry)
{
    auto entity                                     = SG::Entity::Create(a_Registry);
    entity.template GetComponent<Component::Name>() = "Node_" + std::to_string(++GetNbr());
    entity.template AddComponent<Component::Transform>();
    entity.template AddComponent<Component::WorldTransform>();
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

/**
 * @brief Updates world transform, it is recommended to do this before attempting to call LookAt or Orbit
 * @tparam EntityRefType
 * @param a_Node : the Node whose world transform will be updated
 * @param a_UpdateChildren : if true, we will go through the graph to update children's world transform as well
 */
template <typename EntityRefType>
void UpdateWorldTransform(const EntityRefType& a_Node, const Component::Transform& a_BaseTransform, const bool& a_UpdateChildren = true)
{
    const auto& transform = a_Node.template GetComponent<Component::Transform>();
    auto& worldTransform  = a_Node.template GetComponent<Component::WorldTransform>();
    const auto posMat     = glm::translate(a_BaseTransform.GetTransformMatrix(), transform.GetPosition());
    const auto sclMat     = glm::scale(a_BaseTransform.GetScaleMatrix(), transform.GetScale());
    const auto rotMat     = a_BaseTransform.GetRotationMatrix() * transform.GetRotationMatrix();
    const auto pos        = posMat * glm::vec4(0, 0, 0, 1);
    const auto scl        = sclMat * glm::vec4(1, 1, 1, 0);
    const auto rot        = glm::quat_cast(rotMat);
    worldTransform.SetPosition(pos);
    worldTransform.SetScale(scl);
    worldTransform.SetRotation(rot);
    worldTransform.SetUp(rotMat * glm::vec4(transform.GetUp(), 0));
    worldTransform.SetRight(rotMat * glm::vec4(transform.GetRight(), 0));
    worldTransform.SetForward(rotMat * glm::vec4(transform.GetForward(), 0));
    if (a_UpdateChildren && a_Node.HasComponent<Component::Children>()) {
        for (auto& child : a_Node.GetComponent<Component::Children>()) {
            UpdateWorldTransform(child, worldTransform, true);
        }
    }
}

template <typename EntityRefType>
auto LookAt(const EntityRefType& a_Node, const glm::vec3& a_Target)
{
    auto& transform      = a_Node.template GetComponent<Component::Transform>();
    auto& worldTransform = a_Node.template GetComponent<Component::WorldTransform>();
    auto direction       = glm::normalize(a_Target - worldTransform.GetPosition());
    auto directionL      = glm::length(direction);
    auto up              = transform.GetUp();
    if (!(directionL > 0.0001)) {
        transform.SetRotation(glm::quat(1, 0, 0, 0));
        return;
    }
    direction /= directionL;
    if (glm::abs(glm::dot(direction, up)) > 0.9999f) {
        up = glm::vec3(1, 0, 0);
    }
    transform.SetRotation(glm::quatLookAt(direction, up));
    SG::Node::UpdateWorldTransform(a_Node, {}, false);
}

template <typename EntityRefType>
auto LookAt(const EntityRefType& a_Node, const EntityRefType& a_Target)
{
    auto targetPos = a_Target.GetComponent<SG::Component::WorldTransform>().GetPosition();
    return LookAt(a_Node, targetPos);
}

template <typename EntityRefType>
auto Orbit(const EntityRefType& a_Node, const glm::vec3& a_Target, const float& a_Radius, const float& a_Theta, const float& a_Phi)
{
    auto& transform      = a_Node.template GetComponent<Component::Transform>();
    auto cartesianSphere = glm::vec3(
        sin(a_Theta) * cos(a_Phi),
        cos(a_Theta),
        sin(a_Theta) * sin(a_Phi));
    auto cartesianPosition = a_Radius * cartesianSphere;
    transform.SetPosition(a_Target + cartesianPosition);
    LookAt(a_Node, a_Target);
}

template <typename EntityRefType>
auto Orbit(const EntityRefType& a_Node, const EntityRefType& a_Target, const float& a_Radius, const float& a_Theta, const float& a_Phi)
{
    auto targetPos = a_Target.GetComponent<Component::WorldTransform>().GetPosition();
    return Orbit(a_Node, targetPos, a_Radius, a_Theta, a_Phi);
}
}
