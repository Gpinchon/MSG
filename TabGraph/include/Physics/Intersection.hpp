#pragma once

#include <glm/glm.hpp>

class Intersection {
public:
    Intersection(const bool intersects, const float distance, const glm::vec3 normal);
    Intersection(const bool intersects, const float distance);
    bool GetIntersects() const;
    glm::vec3 GetNormal() const;
    glm::vec3 GetPosition() const;
    float GetDistance() const;

private:
    const bool _intersects { false };
    glm::vec3 _normal { 0 };
    glm::vec3 _position { 0 };
    float _distance { 0 };
};