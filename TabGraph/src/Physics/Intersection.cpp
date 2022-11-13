#include "Physics/Intersection.hpp"

Intersection::Intersection(const bool intersects, const float distance, const glm::vec3 normal)
    : _intersects(intersects)
    , _normal(normal)
    , _distance(distance)
{
}

Intersection::Intersection(const bool intersects, const float distance)
    : _intersects(intersects)
    , _distance(distance)
{
}

bool Intersection::GetIntersects() const
{
    return _intersects;
}

glm::vec3 Intersection::GetNormal() const
{
    return _normal;
}

glm::vec3 Intersection::GetPosition() const
{
    return _position;
}

float Intersection::GetDistance() const
{
    return _distance;
}
