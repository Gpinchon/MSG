/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:09
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Physics/BoundingElement.hpp>
#include <Core/Inherit.hpp>
#include <Core/Property.hpp>

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
/** @brief BoundingElement class

	This class provides the Axis Aligned Bounding Box bounding element
	@author: G. Pinchon
*/
namespace TabGraph::Physics {
class BoundingAABB : public Core::Inherit<BoundingElement, BoundingAABB>{
    PROPERTY(glm::vec3, Min, 0);
    PROPERTY(glm::vec3, Max, 0);
public:
    BoundingAABB(glm::vec3 min, glm::vec3 max);
    virtual glm::mat3 LocalInertiaTensor(const float& mass) const override;
    virtual Intersection IntersectRay(const Ray& ray) const override;
    virtual std::set<glm::vec3, compareVec> GetSATAxis(const glm::mat4& transform) const override;
    virtual ProjectionInterval Project(const glm::vec3& axis, const glm::mat4& transform = glm::mat4(1.f)) const override;
    virtual std::vector<glm::vec3> Clip(glm::vec3 axis, const glm::mat4& transform = glm::mat4(1.f)) const override;
};
}
