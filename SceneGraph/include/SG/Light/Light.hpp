/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-17 23:30:50
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Node/Node.hpp> // for Node
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Property.hpp>

#include <glm/glm.hpp> // for glm::vec3
#include <memory> // for shared_ptr, weak_ptr
#include <string> // for string

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
/**
 * @brief defines the base of every lights
 */
class Light : public Inherit<Node, Light> {
public:
    enum class Type {
        Unknown = -1,
        Directional, Point, Spot, HDR, Sky,
        MaxValue
    };
    READONLYPROPERTY(Type, Type, Type::Unknown);
    PROPERTY(bool, CastShadow, false);
    PROPERTY(glm::vec3, Color, 1);
    PROPERTY(float, Intensity, 1);
    PROPERTY(float, Range, std::numeric_limits<float>::infinity());

protected:
    Light();
};
}
