#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Property.hpp>

#include <glm/glm.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Physics {
class RigidBody;
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Physics {
class Collision {
public:
    class CollideesPair {
        READONLYPROPERTY(std::shared_ptr<RigidBody>, First, nullptr);
        READONLYPROPERTY(std::shared_ptr<RigidBody>, Second, nullptr);
    public:
        CollideesPair() = default;
        CollideesPair(const std::shared_ptr<RigidBody>& first, const std::shared_ptr<RigidBody>& second)
        {
            _SetFirst(first);
            _SetSecond(second);
        }
        bool operator==(const CollideesPair& other) const
        {
            if (GetFirst() == other.GetFirst() || GetFirst() == other.GetSecond()) {
                return GetSecond() == other.GetFirst() || GetSecond() == other.GetSecond();
            }
            return false;
        }
    };
    PROPERTY(glm::vec3, Position, std::numeric_limits<float>::infinity());
    PROPERTY(glm::vec3, Normal, 0);
    PROPERTY(float, PenetrationDepth, std::numeric_limits<float>::infinity());
    READONLYPROPERTY(CollideesPair, CollideesPair, );

public:
    Collision() = default;
    Collision(const CollideesPair& collidees, const glm::vec3& position, const glm::vec3& normal, const float& penetrationDepth)
    {
        _SetCollideesPair(collidees);
        SetPosition(position);
        SetNormal(normal);
        SetPenetrationDepth(penetrationDepth);
    }
    bool operator==(const Collision& other)
    {
        return GetCollideesPair() == other.GetCollideesPair() && glm::distance(GetPosition(), other.GetPosition()) <= 0.0001;
    }
};
}