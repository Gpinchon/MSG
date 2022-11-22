/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:24
*/

#include <SG/Light/Light.hpp>

namespace TabGraph::SG {
Light::Light() : Inherit()
{
    static auto s_lightNbr = 0u;
    SetName("Light_" + std::to_string(++s_lightNbr));
    ++s_lightNbr;
}
}