/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:56
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-17 23:30:00
*/
#pragma once

#include <SG/Light/Light.hpp>
#include <SG/Core/Inherit.hpp>

namespace TabGraph::SG {
class LightDirectional : public Inherit<Light, LightDirectional>  {
    PROPERTY(int, ShadowResolution, 1024);
    PROPERTY(float, ShadowBlurRadius, 5.f / 256.f);
    PROPERTY(bool, Infinite, true);
    READONLYPROPERTY(glm::vec3, Direction, glm::vec3(0, 1, 0));

public:
    LightDirectional();
    LightDirectional(const std::string& name, glm::vec3 color, glm::vec3 direction, bool cast_shadow = false);
    void SetDirection(const glm::vec3&);
    glm::vec3 GetHalfSize() const;
    void SetHalfSize(const glm::vec3& halfSize);
    glm::vec3 GetMin() const;
    glm::vec3 GetMax() const;
};
}

