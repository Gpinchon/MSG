#include <MSG/Sphere.hpp>

#include <glm/vec4.hpp>

float Msg::Sphere::Distance(const glm::vec3& a_Position, const glm::mat4x4& a_TransformMatrix) const
{
    glm::vec4 p = a_TransformMatrix * glm::vec4(a_Position - center, 1.f);
    return glm::length(glm::vec3(p) / p.w) - radius;
}