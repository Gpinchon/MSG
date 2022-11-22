/*
* @Author: gpinchon
* @Date:   2020-11-24 21:47:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-19 00:13:17
*/

#include <SG/Light/Directional.hpp>

#include <glm/glm.hpp>

namespace TabGraph::SG {
LightDirectional::LightDirectional()
    : Inherit()
{
    _SetType(Type::Directional);
    static auto s_dirLightNbr = 0u;
    SetName("DirectionalLight_" + std::to_string(++s_dirLightNbr));
    SetHalfSize(glm::vec3(500));
}    
}