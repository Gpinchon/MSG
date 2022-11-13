#include "Physics/BoundingPlane.hpp"
#include "Physics/BoundingSphere.hpp"
#include "Physics/Ray.hpp"

namespace TabGraph::Physics {
BoundingPlane::BoundingPlane(glm::vec3 normal, float distance)
	: Inherit(BoundingElement::Type::Plane)
{
	SetNormal(normal);
	SetDistance(distance);
}

void BoundingPlane::Normalize()
{
	auto magnitude(glm::length(GetNormal()));
	SetNormal(GetNormal() / magnitude);
	SetDistance(GetDistance() / magnitude);
}

Intersection BoundingPlane::IntersectRay(const Ray &ray) const
{
	auto denom = glm::dot(GetNormal(), ray.direction);
	float t(0.f);
	if (denom > 1e-6) {
		auto p0(GetNormal() * GetDistance());
		auto p0l0 = p0 - ray.origin;
		t = glm::dot(p0l0, GetNormal()) / denom;
	}
	return Intersection(t >= 0, t);
}
}
