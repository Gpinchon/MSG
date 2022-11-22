/*
* @Author: gpinchon
* @Date:   2020-11-25 23:58:13
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:23
*/

#include <SG/Light/Spot.hpp>

namespace TabGraph::SG {
LightSpot::LightSpot() : Inherit() {
    _SetType(Type::Spot);
    static size_t s_spotLightNbr = 0u;
    SetName("SpotLight_" + std::to_string(++s_spotLightNbr));
}
}
