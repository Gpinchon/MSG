#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Physics/BoundingElement.hpp>
#include <Core/Inherit.hpp>
#include <Core/Property.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Physics {
class BoundingPlane : public Core::Inherit<BoundingElement, BoundingPlane>
{
	PROPERTY(glm::vec3, Normal, 0);
	PROPERTY(float, Distance, 0);
public:
	BoundingPlane(glm::vec3 normal, float distance);
	virtual Intersection IntersectRay(const Ray &ray) const override;
	void Normalize();
};
}
