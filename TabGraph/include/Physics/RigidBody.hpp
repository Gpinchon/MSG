#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Physics/BoundingElement.hpp>
#include <Common.hpp>
#include <Tools/Tools.hpp>
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>
#include <Core/Property.hpp>
#include <Nodes/Node.hpp>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

/*
** @brief : A RigidBody class for PhysicsEngine
** The Transform Component is the transform that's to be applied after physics calculations 
*/
////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Physics {
class RigidBody : public Core::Inherit<Nodes::Node, RigidBody> {
    PROPERTY(glm::vec3, LastPosition, 0);
    PROPERTY(glm::quat, LastRotation, glm::vec3(0.0, 0.0, 0.0));
    PROPERTY(std::shared_ptr<BoundingElement>, BoundingElement, nullptr);
    PROPERTY(std::shared_ptr<Nodes::Node>, Node, nullptr);
    PROPERTY(bool, Static, false);
    PROPERTY(bool, ApplyGravity, true);
    PROPERTY(float, Restitution, 1);
    PROPERTY(glm::vec3, TotalTorque, 0);
    PROPERTY(glm::vec3, TotalForce, 0);
    PROPERTY(glm::vec3, GravityForce, Common::Gravity());
    PROPERTY(glm::vec3, AngularFactor, 1);
    PROPERTY(glm::vec3, LinearFactor, 1);

    READONLYPROPERTY(glm::vec3, AngularVelocity, 0);
    READONLYPROPERTY(glm::vec3, LinearVelocity, 0);
    READONLYPROPERTY(glm::vec3, InertiaLocal, 1);
    READONLYPROPERTY(glm::vec3, InvInertiaLocal, 1)
    READONLYPROPERTY(float, Mass, 1);
    READONLYPROPERTY(float, InvMass, 1);

public:
    RigidBody(const std::string& name, const std::shared_ptr<Nodes::Node>& node, const std::shared_ptr<BoundingElement>& collider);
    template <typename T, typename U,
        typename = IsSharedPointerOfType<RigidBody, T>,
        typename = IsSharedPointerOfType<RigidBody, U>>
    static bool Collides(const T& a, const U& b);

    glm::mat4 GetCurrentTransform();
    
    /** @arg impulse : the impulse in world space */
    void ApplyAngularImpulse(const glm::vec3& impulse);
    /** @arg impulse : the impulse in world space */
    void ApplyLinearImpulse(const glm::vec3& impulse);

    void SetMass(const float mass);

    void RigidBody::SetLinearVelocity(const glm::vec3 velocity);
    void RigidBody::SetAngularVelocity(const glm::vec3 velocity);

    glm::quat AngularSpin() const;
    glm::mat3 InertiaTensor();
    glm::mat3 InvInertiaTensor();
    void SetInertiaLocal(glm::vec3 inertia);
    void ApplyTorque(glm::vec3 torque);
    /** Apply a force on the Node at a certain location, can generate torque */
    void ApplyLocalForce(glm::vec3 force, glm::vec3 forceLocation);
    /** Apply a force at the center of the object, won't generate torque */
    void ApplyCentralForce(glm::vec3 pushDirection);
    void ApplyCentralPush(glm::vec3 pushDirection);
    void ApplyLocalPush(glm::vec3 pushDirection, glm::vec3 pushLocation);
    void ApplyWorldPush(glm::vec3 pushDirection, glm::vec3 pushLocation, glm::vec3 originalPosition);
    void IntegrateVelocities(float step);

    Intersection Collides(const std::shared_ptr<RigidBody>& objectB);
};
}
