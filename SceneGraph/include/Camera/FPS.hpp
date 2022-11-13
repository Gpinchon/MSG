/*
* @Author: gpinchon
* @Date:   2019-07-15 10:36:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:30:46
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Camera/Camera.hpp>
#include <Camera/Projection.hpp>
#include <Core/Inherit.hpp>
#include <Core/Property.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class CameraFPS : public Inherit<Camera, CameraFPS> {
    READONLYPROPERTY(float, Yaw, 0);
    READONLYPROPERTY(float, Pitch, 0);
    READONLYPROPERTY(float, Roll, 0);

public:
    CameraFPS(const std::string& name, CameraProjection proj = CameraProjection::PerspectiveInfinite()) : Inherit(name, proj) {}
    void SetYaw(float);
    void SetPitch(float);
    void SetRoll(float);
};
}
