/*
* @Author: gpinchon
* @Date:   2020-11-24 21:47:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-19 00:13:17
*/

#include <Light/Directional.hpp>

#include <glm/glm.hpp>

namespace TabGraph::SG {
LightDirectional::LightDirectional()
    : Inherit()
{
    static auto s_dirLightNbr = 0u;
    SetName("DirectionalLight_" + std::to_string(s_dirLightNbr));
    ++s_dirLightNbr;
}

LightDirectional::LightDirectional(const std::string& name, glm::vec3 color, glm::vec3 direction, bool cast_shadow)
    : LightDirectional()
{
    SetName(name);
    SetColor(color);
    SetDirection(direction);
    SetHalfSize(glm::vec3(500));
    SetCastShadow(cast_shadow);
}

void LightDirectional::SetDirection(const glm::vec3& direction)
{
    _SetDirection(normalize(direction));
}

glm::vec3 LightDirectional::GetHalfSize() const
{
    return GetLocalScale() / 2.f;
}

void LightDirectional::SetHalfSize(const glm::vec3& halfSize)
{
    SetLocalScale(halfSize * 2.f);
}

glm::vec3 LightDirectional::GetMin() const
{
    return GetWorldPosition() - GetHalfSize();
}

glm::vec3 LightDirectional::GetMax() const
{
    return GetWorldPosition() + GetHalfSize();
}
}