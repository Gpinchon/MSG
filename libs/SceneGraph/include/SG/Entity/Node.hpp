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
#include <SG/Component/BoundingVolume.hpp>
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
#define NODE_COMPONENTS ENTITY_COMPONENTS, SG::Component::Transform, SG::Component::BoundingVolume, SG::Component::Parent
/** @return the total nbr of Nodes created since start-up */
uint32_t& GetNbr();
template <typename RegistryType>
auto Create(const RegistryType& a_Registry)
{
    auto entity                                     = SG::Entity::Create(a_Registry);
    entity.template GetComponent<Component::Name>() = "Node_" + std::to_string(++GetNbr());
    entity.template AddComponent<Component::Transform>();
    entity.template AddComponent<Component::BoundingVolume>();
    entity.template AddComponent<Component::Parent>();
    return entity;
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
auto RemoveParent(const EntityRefType& a_Child)
{
    auto registry  = a_Child.GetRegistry();
    auto& parentID = a_Child.template GetComponent<Component::Parent>();
    if (!parentID || !registry->IsAlive(parentID))
        return;
    registry->GetEntityRef(parentID).template GetComponent<Component::Children>().erase(a_Child);
    parentID.reset();
}

/**
 * @brief sets a_Parent to be parent of a_Child and adds a_Child in parent's Children
 * @param a_Child : the Node whose parent is to be set
 * @param a_Parent : a_Child's new parent
 */
template <typename EntityRefType>
auto SetParent(const EntityRefType& a_Child, const EntityRefType& a_Parent)
{
    RemoveParent(a_Child);
    auto& parent   = a_Child.template GetComponent<Component::Parent>();
    auto& children = a_Parent.template GetComponent<Component::Children>();
    parent         = typename EntityRefType::IDType(a_Parent);
    children.insert(a_Child);
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
    auto& transform = a_Node.template GetComponent<Component::Transform>();
    transform.UpdateWorld(a_BaseTransform);
    if (a_UpdateChildren && a_Node.template HasComponent<Component::Children>()) {
        for (auto& child : a_Node.template GetComponent<Component::Children>()) {
            UpdateWorldTransform(child, transform, true);
        }
    }
}

template <typename EntityRefType>
auto LookAt(const EntityRefType& a_Node, const glm::vec3& a_Target)
{
    auto& transform = a_Node.template GetComponent<Component::Transform>();
    auto direction  = glm::normalize(a_Target - transform.GetWorldPosition());
    auto directionL = glm::length(direction);
    auto up         = transform.GetLocalUp();
    if (!(directionL > 0.0001)) {
        transform.SetLocalRotation(glm::quat(1, 0, 0, 0));
        return;
    }
    direction /= directionL;
    if (glm::abs(glm::dot(direction, up)) > 0.9999f) {
        up = glm::vec3(1, 0, 0);
    }
    transform.SetLocalRotation(glm::quatLookAt(direction, up));
}

template <typename EntityRefType>
auto LookAt(const EntityRefType& a_Node, const EntityRefType& a_Target)
{
    auto targetPos = a_Target.template GetComponent<SG::Component::Transform>().GetWorldPosition();
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
    transform.SetLocalPosition(a_Target + cartesianPosition);
    UpdateWorldTransform(a_Node, {}, false);
    LookAt(a_Node, a_Target);
}

template <typename EntityRefType>
auto Orbit(const EntityRefType& a_Node, const EntityRefType& a_Target, const float& a_Radius, const float& a_Theta, const float& a_Phi)
{
    auto targetPos = a_Target.template GetComponent<Component::Transform>().GetWorldPosition();
    return Orbit(a_Node, targetPos, a_Radius, a_Theta, a_Phi);
}
}
