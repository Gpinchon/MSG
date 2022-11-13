#include <Physics/BoundingElement.hpp>
#include <Physics/BoundingSphere.hpp>
#include <Physics/BoundingAABB.hpp>
#include <Physics/BoundingPlane.hpp>
#include <Physics/BoundingMesh.hpp>
#include <Physics/BoundingBox.hpp>
#include <Debug.hpp>
#include <functional>

namespace TabGraph::Physics{
BoundingElement::BoundingElement(BoundingElement::Type type) : _type(type)
{
	//auto errorCallback(Callback<Intersection(const BoundingElement &)>::Create(&BoundingElement::IntersectError, this, std::placeholders::_1));
	//_intersectionCallback.at(Type::Sphere) = errorCallback;
	//_intersectionCallback.at(Type::AABB) = errorCallback;
	//_intersectionCallback.at(Type::Plane) = errorCallback;
	//_intersectionCallback.at(Type::Mesh) = errorCallback;
	//_intersectionCallback.at(Type::Box) = errorCallback;
}
/*
void BoundingElement::SetIntersectionCallback(BoundingElement::Type type, Intersection (*function)(const BoundingElement &))
{
	auto callback(Callback<Intersection(const BoundingElement &)>::Create(function, this, std::placeholders::_1));
	_intersectionCallback.at(type) = callback;
}
*/
/*
Intersection BoundingElement::Intersect(const std::shared_ptr<BoundingElement> &other) const
{
	std::cout << __FUNCTION__ << std::endl;
	std::cout << GetType() << std::endl;
	std::cout << other->GetType() << std::endl;
	switch (other->GetType()) {
		case (Sphere) :
			return IntersectSphere(std::static_pointer_cast<BoundingSphere>(other));
		case (AABB) :
			return IntersectAABB(std::static_pointer_cast<BoundingAABB>(other));
		case (Plane) :
			return IntersectPlane(std::static_pointer_cast<BoundingPlane>(other));
		case (Geometry) :
			return IntersectGeometry(std::static_pointer_cast<BoundingMesh>(other));
		case (Box) :
			return IntersectBox(std::static_pointer_cast<BoundingBox>(other));
		default :
			return IntersectError(std::static_pointer_cast<BoundingElement>(other));
	}
	//const auto &callback(_intersectionCallback.at(other->GetType()));
	//if (callback != nullptr)
	//	return (*callback)(other);
	//return IntersectError(other);
}*/

Intersection BoundingElement::IntersectError(const std::shared_ptr<BoundingElement> &other) const
{
	std::cerr << "[ERROR] : Intersection function not implemented between types "
		<< int(GetType()) << " and " << int(other->GetType()) << std::endl;
	return Intersection(false, NAN);
}

Intersection BoundingElement::IntersectRay(const Ray &) const
{
	std::cerr << "[ERROR] : Intersection function not implemented between type "
		<< int(GetType()) << " and Ray"<< std::endl;
	return Intersection(false, NAN);
}
/*
Intersection BoundingElement::IntersectSphere(const std::shared_ptr<BoundingSphere> &) const
{
	std::cerr << "[ERROR] : Intersection function not implemented between type "
		<< GetType() << " and BoundingSphere"<< std::endl;
	return Intersection(false, NAN);
}
Intersection BoundingElement::IntersectAABB(const std::shared_ptr<BoundingAABB> &) const
{
	std::cerr << "[ERROR] : Intersection function not implemented between type "
		<< GetType() << " and BoundingAABB"<< std::endl;
	return Intersection(false, NAN);
}
Intersection BoundingElement::IntersectPlane(const std::shared_ptr<BoundingPlane> &) const
{
	std::cerr << "[ERROR] : Intersection function not implemented between type "
		<< GetType() << " and BoundingPlane"<< std::endl;
	return Intersection(false, NAN);
}
Intersection BoundingElement::IntersectGeometry(const std::shared_ptr<BoundingMesh> &) const
{
	std::cerr << "[ERROR] : Intersection function not implemented between type "
		<< GetType() << " and BoundingMesh"<< std::endl;
	return Intersection(false, NAN);
}
Intersection BoundingElement::IntersectBox(const std::shared_ptr<BoundingBox> &) const
{
	std::cerr << "[ERROR] : Intersection function not implemented between type "
		<< GetType() << " and BoundingBox"<< std::endl;
	return Intersection(false, NAN);
}

void BoundingElement::Transform(const glm::vec3 &, const glm::quat &, const glm::vec3 &)
{
	std::cerr << "[ERROR] : Transform function not implemented for type " << GetType() << std::endl;
}
*/

BoundingElement::Type BoundingElement::GetType() const
{
	return _type;
}
}