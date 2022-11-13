#include "Physics/BoundingSphere.hpp"
#include "Physics/IntersectFunctions.hpp"
#include "Physics/Ray.hpp"

#include <glm/gtx/matrix_decompose.hpp>

namespace TabGraph::Physics {
BoundingSphere::BoundingSphere(const glm::vec3& center, float radius)
    : Inherit(BoundingElement::Type::Sphere)
{
    SetCenter(center);
    SetRadius(radius);
}

glm::mat3 BoundingSphere::LocalInertiaTensor(const float& mass) const
{
    auto alpha = 2 / 3.f * mass * GetRadius();
    return glm::mat3(alpha);
}

glm::vec3 sphere_normal(glm::vec3 position, glm::vec3 intersectPosition, float radius)
{
    return normalize((position - intersectPosition) / radius);
}

bool intersect_test(float t0, float t1)
{
    return (t0 > 0.f || t1 > 0.f);
}

bool solve_quadratic(float a, float b, float c, float* t)
{
    float discrim;
    float q;

    discrim = (b * b - 4.f * a * c);
    if (discrim < 0)
        return false;
    q = -.5f * (b < 0 ? (b - sqrtf(discrim)) : (b + sqrtf(discrim)));
    auto t0 = q / a;
    auto t1 = c / q;
    if (t0 > t1) {
        q = t0;
        t0 = t1;
        t1 = q;
    }
    *t = glm::min(t0, t1);
    return intersect_test(t0, t1);
}

Intersection SphereIntersection(const Ray ray, glm::vec3 position, float /*radius*/, float radius2)
{
    glm::vec3 eye;
    float distance(0.f);

    eye = ray.origin - position;
    if (!(solve_quadratic(
            glm::dot(ray.direction, ray.direction),
            glm::dot(eye, ray.direction) * 2.0,
            glm::dot(eye, eye) - radius2,
            &distance)))
        return Intersection(false, distance);
    return (Intersection(true, distance));
}

Intersection BoundingSphere::IntersectRay(const Ray& ray) const
{
    return SphereIntersection(ray, GetCenter(), GetRadius(), GetRadius() * GetRadius());
}

inline auto spherical_to_cartesian(float parallel, float meridian)
{
    auto const cos_azimuth = std::cos(parallel);
    auto const sin_azimuth = std::sin(parallel);
    auto const cos_elevation = std::cos(meridian);
    auto const sin_elevation = std::sin(meridian);

    return glm::vec3 { cos_azimuth * cos_elevation, sin_azimuth * cos_elevation, sin_elevation };
}

std::set<glm::vec3, compareVec> BoundingSphere::GetSATAxis(const glm::mat4& /*transform*/) const
{
    static std::set<glm::vec3, compareVec> axis;
    if (axis.empty()) {
        auto granularity(10);
        float sectorStep = 2 * M_PI / float(granularity);
        float stackStep = M_PI / float(granularity);
        for (auto x = 0; x < granularity; ++x) {
            auto parallel(M_PI / 2 - x * stackStep);
            for (auto y = 0; y < granularity; ++y) {
                auto meridian(y * sectorStep);
                auto n(normalize(spherical_to_cartesian(parallel, meridian)));
                axis.insert(n);
            }
        }
    }
    return axis;
}

BoundingElement::ProjectionInterval BoundingSphere::Project(const glm::vec3& axis, const glm::mat4& transform) const
{
    auto vec = axis;
    if (length(vec) < 0.0001) {
        vec = glm::vec3(1, 0, 0);
    } else
        vec = normalize(vec);
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, scale, rotation, translation, skew, perspective);
    glm::vec3 minV(glm::vec4(GetCenter() + translation - GetRadius() * vec, 1.f));
    glm::vec3 maxV(glm::vec4(GetCenter() + translation + GetRadius() * vec, 1.f));
    return ProjectionInterval(vec, minV, maxV);
}

std::vector<glm::vec3> BoundingSphere::Clip(glm::vec3 /*axis*/, const glm::mat4& /*transform*/) const
{
    std::vector<glm::vec3> output;
    return output;
}
}