/*
* @Author: gpinchon
* @Date:   2021-03-14 22:12:40
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:25
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Light/Light.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Property.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Image;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class LightHDR : public Inherit<Light, LightHDR> {
    PROPERTY(std::shared_ptr<Image>, Image, nullptr);
    PROPERTY(bool, Infinite, true);

public:
    LightHDR(const std::string& name, std::shared_ptr<Image> image);
    inline glm::vec3 GetHalfSize() const
    {
        return GetLocalScale() / 2.f;
    }
    inline void SetHalfSize(const glm::vec3& halfSize)
    {
        SetLocalScale(halfSize * 2.f);
    }
    inline glm::vec3 GetMin() const
    {
        return GetWorldPosition() - GetHalfSize();
    }
    inline glm::vec3 GetMax() const
    {
        return GetWorldPosition() + GetHalfSize();
    }
};

}
