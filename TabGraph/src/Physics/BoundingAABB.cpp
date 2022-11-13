/*
* @Author: gpinchon
* @Date:   2019-05-15 13:53:32
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:11
*/

#include <Physics/BoundingAABB.hpp>
#include <Physics/Ray.hpp>

#include <array>
#include <glm/gtx/matrix_decompose.hpp>

namespace TabGraph::Physics {
BoundingAABB::BoundingAABB(glm::vec3 minV, glm::vec3 maxV)
    : Inherit(Type::AABB)
{
    SetMin(minV);
    SetMax(maxV);
}

std::set<glm::vec3, compareVec> BoundingAABB::GetSATAxis(const glm::mat4& /*transform*/) const
{
    static std::set<glm::vec3, compareVec> axis({ glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1),
        glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0), glm::vec3(0, 0, -1) });
    return axis;
}

glm::mat3 BoundingAABB::LocalInertiaTensor(const float& mass) const
{
    auto w = fabs(GetMax().x - GetMin().x);
    auto w2 = w * w;
    auto h = fabs(GetMax().y - GetMin().y);
    auto h2 = h * h;
    auto d = fabs(GetMax().z - GetMin().z);
    auto d2 = d * d;
    auto m = 1 / 12.f * mass;
    return {
        m * (h2 + d2), 0, 0,
        0, m * (w2 + d2), 0,
        0, 0, m * (w2 + h2)
    };
}

BoundingElement::ProjectionInterval BoundingAABB::Project(const glm::vec3& axis, const glm::mat4& transform) const
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
    {
        static std::array<glm::vec3, 8> vertex;
        vertex.at(0) = GetMax() + translation;
        vertex.at(1) = glm::vec3(GetMin().x, GetMin().y, GetMax().z) + translation;
        vertex.at(2) = glm::vec3(GetMin().x, GetMax().y, GetMax().z) + translation;
        vertex.at(3) = glm::vec3(GetMin().x, GetMax().y, GetMin().z) + translation;
        vertex.at(4) = glm::vec3(GetMax().x, GetMax().y, GetMin().z) + translation;
        vertex.at(5) = glm::vec3(GetMax().x, GetMin().y, GetMin().z) + translation;
        vertex.at(6) = glm::vec3(GetMax().x, GetMin().y, GetMax().z) + translation;
        vertex.at(7) = GetMin() + translation;
        glm::vec3 minV;
        float minDot { std::numeric_limits<float>::max() };
        glm::vec3 maxV;
        float maxDot { std::numeric_limits<float>::lowest() };
        for (auto v : vertex) {
            auto dotProd = dot(v, vec);
            if (dotProd < minDot) {
                minDot = dotProd;
                minV = v;
            }
            if (dotProd > maxDot) {
                maxDot = dotProd;
                maxV = v;
            }
        }
        return ProjectionInterval(vec, minV, maxV);
    }
}

static inline auto IsInFront(glm::vec3 axis, glm::vec3 point)
{
    return dot(axis, point) > 0;
}

static inline auto IntersectionPoint(glm::vec3 axis, glm::vec3 a, glm::vec3 b)
{
    auto dir(normalize(a - b));
    float denom(dot(axis, dir));
    auto p(-a);
    auto t(dot(p, axis) / denom);
    return a + t * dir;
}

std::vector<glm::vec3> BoundingAABB::Clip(glm::vec3 axis, const glm::mat4& transform) const
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    std::vector<glm::vec3> output;
    static std::array<glm::vec3, 7> vertex;

    glm::decompose(transform, scale, rotation, translation, skew, perspective);
    vertex.at(0) = GetMax() + translation;
    vertex.at(1) = glm::vec3(GetMin().x, GetMin().y, GetMax().z) + translation;
    vertex.at(2) = glm::vec3(GetMin().x, GetMax().y, GetMax().z) + translation;
    vertex.at(3) = glm::vec3(GetMin().x, GetMax().y, GetMin().z) + translation;
    vertex.at(4) = glm::vec3(GetMax().x, GetMax().y, GetMin().z) + translation;
    vertex.at(5) = glm::vec3(GetMax().x, GetMin().y, GetMin().z) + translation;
    vertex.at(6) = glm::vec3(GetMax().x, GetMin().y, GetMax().z) + translation;
    auto startingPoint(vertex.at(6));
    for (const auto& v : vertex) {
        glm::vec3 endPoint(glm::vec4(v, 1.f));
        if (IsInFront(axis, startingPoint) && IsInFront(axis, endPoint)) {
            output.push_back(startingPoint);
            output.push_back(endPoint);
        } else if (IsInFront(axis, startingPoint) && !IsInFront(axis, endPoint)) {
            output.push_back(IntersectionPoint(axis, startingPoint, endPoint));
            output.push_back(startingPoint);
        } else if (!IsInFront(axis, startingPoint) && IsInFront(axis, endPoint)) {
            output.push_back(IntersectionPoint(axis, endPoint, startingPoint));
            output.push_back(endPoint);
        }
        startingPoint = endPoint;
    }
    return output;
}

static inline Intersection CreateAABBRayIntersection(const bool intersects, const float distance, const BoundingAABB* aabb, const Ray& ray)
{
    auto position = ray.direction * distance + ray.origin;
    auto c = (aabb->GetMin() + aabb->GetMax()) * 0.5f;
    auto d = (aabb->GetMin() - aabb->GetMax()) * 0.5f;
    auto p = position - c;
    auto bias = 1.000001;
    auto normal = glm::vec3(p.x / abs(d.x) * bias,
        p.y / abs(d.y) * bias,
        p.z / abs(d.z) * bias);
    normal = glm::normalize(normal);
    return Intersection(intersects, distance, normal);
}

Intersection BoundingAABB::IntersectRay(const Ray& ray) const
{
    float tmin = (GetMin().x - ray.origin.x) / ray.direction.x;
    float tmax = (GetMax().x - ray.origin.x) / ray.direction.x;

    if (tmin > tmax)
        std::swap(tmin, tmax);

    float tymin = (GetMin().y - ray.origin.y) / ray.direction.y;
    float tymax = (GetMax().y - ray.origin.y) / ray.direction.y;

    if (tymin > tymax)
        std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return CreateAABBRayIntersection(false, tmin, this, ray);

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (GetMin().z - ray.origin.z) / ray.direction.z;
    float tzmax = (GetMax().z - ray.origin.z) / ray.direction.z;

    if (tzmin > tzmax)
        std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return CreateAABBRayIntersection(false, tmin, this, ray);
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    return CreateAABBRayIntersection(true, tmin, this, ray);
}
}