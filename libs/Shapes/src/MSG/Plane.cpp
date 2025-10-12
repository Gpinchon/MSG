#include <MSG/Plane.hpp>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

Msg::Plane::Plane()
    : glm::vec4(0, 1, 0, 0) { };

Msg::Plane::Plane(const glm::vec3& a_Point, const glm::vec3& a_Normal)
    : glm::vec4(glm::normalize(a_Normal), glm::length(a_Point))
{
}

void Msg::Plane::SetNormal(const glm::vec3& a_Normal)
{
    x = a_Normal.x;
    y = a_Normal.y;
    z = a_Normal.z;
}
