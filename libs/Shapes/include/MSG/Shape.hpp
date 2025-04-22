#pragma once

#include <MSG/Cube.hpp>
#include <MSG/Sphere.hpp>

#include <variant>

namespace MSG {
struct Shape : std::variant<Cube, Sphere> {
    float Distance(const glm::vec3& a_Position, const glm::mat4x4& a_TransformMatrix = { 1.f }) const
    {
        return std::visit([a_Position, a_TransformMatrix](auto& a_Shape) { return a_Shape.Distance(a_Position, a_TransformMatrix); }, *this);
    }
    bool IsInside(const glm::vec3& a_Position, const glm::mat4x4& a_TransformMatrix = { 1.f }) const
    {
        return Distance(a_Position, a_TransformMatrix) <= 0.f;
    }
    glm::vec3 Min() const
    {
        return std::visit([](auto& a_Shape) { return a_Shape.Min(); }, *this);
    }
    glm::vec3 Max() const
    {
        return std::visit([](auto& a_Shape) { return a_Shape.Max(); }, *this);
    }
};
}