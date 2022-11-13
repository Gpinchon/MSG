/*
* @Author: gpinchon
* @Date:   2020-08-03 15:24:11
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-03 17:12:30
*/

#include <Physics/CollisionAlgorithmSAT.hpp>
#include <Physics/Collision.hpp>
#include <Physics/RigidBody.hpp>

namespace TabGraph::Physics {
bool CheckAxis(const std::shared_ptr<RigidBody>& a, const std::shared_ptr<RigidBody>& b, const glm::vec3 axis, Collision& out)
{
    auto collidees = Collision::CollideesPair(a, b);
    auto& transformA = a->GetCurrentTransform();
    auto& transformB = b->GetCurrentTransform();
    auto projectA = a->GetBoundingElement()->Project(axis, transformA);
    auto projectB = b->GetBoundingElement()->Project(axis, transformB);
    auto A = projectA.MinDot();
    auto B = projectA.MaxDot();
    auto C = projectB.MinDot();
    auto D = projectB.MaxDot();
    if (A <= C && B >= C) {
        auto normal = axis;
        auto penetration = C - B;
        auto position = projectA.Max() + normal * penetration;
        out = Collision(collidees, position, normal, penetration);
        return true;
    }
    if (C <= A && D >= A) {
        auto normal = -axis;
        auto penetration = A - D;
        auto position = projectA.Min() + normal * penetration;
        out = Collision(collidees, position, normal, penetration);
        return true;
    }
    return false;
}

bool CollisionAlgorithmSAT::Collides(const std::shared_ptr<RigidBody>& a, const std::shared_ptr<RigidBody>& b, Collision& out)
{

    auto boundingElementA(a->GetBoundingElement());
    auto boundingElementB(b->GetBoundingElement());
    if (boundingElementA == nullptr || boundingElementB == nullptr)
        return false;
    auto& transformA = a->GetCurrentTransform();
    auto& transformB = b->GetCurrentTransform();
    auto axisA(boundingElementA->GetSATAxis(transformA));
    auto axisB(boundingElementB->GetSATAxis(transformB));

    std::set<glm::vec3, compareVec> possibleAxis;

    for (const auto& norm1 : axisA) {
        for (const auto& norm2 : axisB) {
            possibleAxis.insert(cross(norm1, norm2));
        }
    }
    possibleAxis.insert(axisA.begin(), axisA.end());
    possibleAxis.insert(axisB.begin(), axisB.end());
    Collision bestCollision;
    for (const auto& axis : possibleAxis) {
        Collision collision;
        if (!CheckAxis(a, b, axis, collision)) //if true, we found separating Axis
            return false;
        if (bestCollision.GetPenetrationDepth() == std::numeric_limits<float>::infinity() || collision.GetPenetrationDepth() >= bestCollision.GetPenetrationDepth())
            bestCollision = collision;
    }
    std::cout << __FUNCTION__ << " D : " << bestCollision.GetPenetrationDepth() << std::endl;
    std::cout << __FUNCTION__ << " N : " << bestCollision.GetNormal().x << " " << bestCollision.GetNormal().y << " " << bestCollision.GetNormal().z << std::endl;
    std::cout << __FUNCTION__ << " P : " << bestCollision.GetPosition().x << " " << bestCollision.GetPosition().y << " " << bestCollision.GetPosition().z << std::endl;
    //We haven't found a separating axis
    out = bestCollision;
    return true;
}
}