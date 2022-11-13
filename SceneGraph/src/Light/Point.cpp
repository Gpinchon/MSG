/*
* @Author: gpinchon
* @Date:   2020-11-25 23:39:55
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-19 00:13:30
*/
#include <Light/Point.hpp>

namespace TabGraph::SG {
LightPoint::LightPoint(const std::string& name, glm::vec3 color, bool cast_shadow)
    : Inherit(name)
{
    SetColor(color);
    SetCastShadow(cast_shadow);
}
}
