/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:56
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-17 23:30:00
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Light/Light.hpp>
#include <SG/Core/Inherit.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class LightDirectional : public Inherit<Light, LightDirectional>  {
public:
    LightDirectional();
    LightDirectional(const std::string& a_Name) : LightDirectional() {
        SetName(a_Name);
    }
    inline auto GetHalfSize() const {
        return GetLocalScale() / 2.f;
    }
    inline void SetHalfSize(const glm::vec3& a_HalfSize) {
        SetLocalScale(a_HalfSize * 2.f);
    }
    inline auto GetMin() const {
        return GetWorldPosition() - GetHalfSize();
    }
    inline auto GetMax() const {
        return GetWorldPosition() + GetHalfSize();
    }
};
}

