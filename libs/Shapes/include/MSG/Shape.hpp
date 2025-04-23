#pragma once

#include <MSG/Cube.hpp>
#include <MSG/Sphere.hpp>

#include <variant>
#include <vector>

namespace MSG {
struct Shape : std::variant<Cube, Sphere> {
    using std::variant<Cube, Sphere>::variant;
    float Distance(const glm::vec3& a_Position, const glm::mat4x4& a_TransformMatrix = { 1.f }) const;
    bool IsInside(const glm::vec3& a_Position, const glm::mat4x4& a_TransformMatrix = { 1.f }) const;
    glm::vec3 Min() const;
    glm::vec3 Max() const;
};

enum class ShapeCombinationOp {
    Add,
    Substract,
    Intersect,
    Xor
};

struct ShapeCombinationShape : Shape {
    ShapeCombinationOp op = ShapeCombinationOp::Add;
};

struct ShapeCombination : std::vector<ShapeCombinationShape> {
    float Distance(const glm::vec3& a_Position, const glm::mat4x4& a_TransformMatrix = { 1.f }) const;
    bool IsInside(const glm::vec3& a_Position, const glm::mat4x4& a_TransformMatrix = { 1.f }) const;
    glm::vec3 Min() const;
    glm::vec3 Max() const;
};

static inline float ShapeUnion(float d1, float d2) { return glm::min(d1, d2); }
static inline float ShapeSubtraction(float d1, float d2) { return glm::max(-d1, d2); }
static inline float ShapeIntersection(float d1, float d2) { return glm::max(d1, d2); }
static inline float ShapeXor(float d1, float d2) { return glm::max(ShapeUnion(d1, d2), -ShapeIntersection(d1, d2)); }
}