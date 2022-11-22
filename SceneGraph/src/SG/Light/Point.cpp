/*
* @Author: gpinchon
* @Date:   2020-11-25 23:39:55
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-19 00:13:30
*/
#include <SG/Light/Point.hpp>

namespace TabGraph::SG {
LightPoint::LightPoint() : Inherit()
{
    _SetType(Type::Point);
    static auto s_pointLightNbr = 0u;
    SetName("PointLight_" + std::to_string(++s_pointLightNbr));
}
}
