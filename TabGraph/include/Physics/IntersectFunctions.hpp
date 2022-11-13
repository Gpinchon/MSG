#include <Physics/BoundingAABB.hpp>
#include <Physics/BoundingBox.hpp>
#include <Physics/BoundingMesh.hpp>
#include <Physics/BoundingPlane.hpp>
#include <Physics/BoundingSphere.hpp>
#include <Shapes/Geometry.hpp>

#include <glm/gtx/component_wise.hpp>

namespace TabGraph::Physics {
inline Intersection IntersectFunction(const glm::vec3& point, const BoundingAABB& box)
{
	auto underMax(glm::lessThanEqual(point, box.GetMax()));
	auto aboveMin(glm::greaterThanEqual(point, box.GetMin()));
	return Intersection(glm::all(underMax) && glm::all(aboveMin), 0.f);
}

inline Intersection IntersectFunction(const BoundingSphere& a, const BoundingSphere& b)
{
	float radiusDistance = a.GetRadius() + b.GetRadius();
	float centerDistance = glm::distance(a.GetCenter(), b.GetCenter());
	float distance = centerDistance - radiusDistance;
	return Intersection(centerDistance < radiusDistance, distance);
}

inline Intersection isPointInsideSphere(const glm::vec3& point, const BoundingSphere& sphere)
{
	auto distance(glm::distance(point, sphere.GetCenter()));
	return Intersection(distance < sphere.GetRadius(), distance);
}

inline Intersection IntersectFunction(const BoundingAABB& a, const BoundingAABB& b)
{
	auto distance1(b.GetMin() - a.GetMax());
	auto distance2(a.GetMin() - b.GetMax());
	auto distance(glm::max(distance1, distance2));
	auto maxDistance(glm::compMax(distance));
	return Intersection(maxDistance < 0, maxDistance);
}
}
