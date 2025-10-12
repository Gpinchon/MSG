#include <MSG/Shape.hpp>

float Msg::Shape::Distance(const glm::vec3& a_Position, const glm::mat4x4& a_TransformMatrix) const
{
    return std::visit([a_Position, a_TransformMatrix](auto& a_Shape) { return a_Shape.Distance(a_Position, a_TransformMatrix); }, *this);
}
bool Msg::Shape::IsInside(const glm::vec3& a_Position, const glm::mat4x4& a_TransformMatrix) const
{
    return Distance(a_Position, a_TransformMatrix) <= 0.f;
}
glm::vec3 Msg::Shape::Min() const
{
    return std::visit([](auto& a_Shape) { return a_Shape.Min(); }, *this);
}
glm::vec3 Msg::Shape::Max() const
{
    return std::visit([](auto& a_Shape) { return a_Shape.Max(); }, *this);
}

float Msg::ShapeCombination::Distance(const glm::vec3& a_Position, const glm::mat4x4& a_TransformMatrix) const
{
    float dist;
    bool first = true;
    for (auto& shape : *this) {
        if (first) [[unlikely]] {
            dist  = shape.Distance(a_Position, a_TransformMatrix);
            first = false;
        } else {
            switch (shape.op) {
            case ShapeCombinationOp::Add:
                dist = ShapeUnion(shape.Distance(a_Position, a_TransformMatrix), dist);
                break;
            case ShapeCombinationOp::Substract:
                dist = ShapeSubtraction(shape.Distance(a_Position, a_TransformMatrix), dist);
                break;
            case ShapeCombinationOp::Intersect:
                dist = ShapeIntersection(shape.Distance(a_Position, a_TransformMatrix), dist);
                break;
            case ShapeCombinationOp::Xor:
                dist = ShapeXor(shape.Distance(a_Position, a_TransformMatrix), dist);
                break;
            }
        }
    }
    return dist;
}
bool Msg::ShapeCombination::IsInside(const glm::vec3& a_Position, const glm::mat4x4& a_TransformMatrix) const
{
    return Distance(a_Position, a_TransformMatrix) <= 0.f;
}
glm::vec3 Msg::ShapeCombination::Min() const
{
    glm::vec3 shapeMin = glm::vec3(std::numeric_limits<float>::max());
    for (auto& shape : *this)
        shapeMin = glm::min(shapeMin, shape.Min());
    return shapeMin;
}
glm::vec3 Msg::ShapeCombination::Max() const
{
    glm::vec3 shapeMax = glm::vec3(std::numeric_limits<float>::min());
    for (auto& shape : *this)
        shapeMax = glm::max(shapeMax, shape.Max());
    return shapeMax;
}
