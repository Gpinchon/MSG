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
#include <Common.hpp>
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

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
