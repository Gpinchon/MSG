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
#include <SG/Camera/Camera.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::FPSCamera {
struct Settings {
    float yaw{ 0 };
    float pitch{ 0 };
    float roll{ 0 };
};
#define FPSCAMERA_COMPONENTS CAMERA_COMPONENTS, SG::FPSCamera::Settings

/** @return the total nbr of FPS Cameras created since start-up */
uint32_t& GetNbr();
template<typename RegistryType>
auto Create(const RegistryType& a_Registry) {
    auto entity = SG::Camera::Create(a_Registry);
    entity.GetComponent<SG::Name>() = "FPSCamera_" + std::to_string(++GetNbr());
    entity.AddComponent<Settings>();
    return entity;
}

static inline glm::quat GetRotationFromYPR(const float Yaw, const float Pitch, const float Roll)
{
    auto radPitch(glm::radians(Pitch));
    auto radYaw(glm::radians(Yaw));
    auto radRoll(glm::radians(Roll));
    glm::quat quatRoll = glm::angleAxis(radRoll, Common::Forward());
    glm::quat quatPitch = glm::angleAxis(radPitch, Common::Right());
    glm::quat quatYaw = glm::angleAxis(radYaw, Common::Up());
    glm::quat rotation = quatYaw * quatPitch * quatRoll;
    return glm::normalize(rotation);
}
template<typename EntityRefType>
void SetYaw(const EntityRefType& a_Entity, float a_Yaw) {
    auto& transform = a_Entity.GetComponent<Transform>();
    auto& settings = a_Entity.GetComponent<Settings>();
    settings.yaw = a_Yaw;
    transform.rotation = GetRotationFromYPR(settings.yaw, settings.pitch, settings.roll);
}
template<typename EntityRefType>
void SetPitch(const EntityRefType& a_Entity, float a_Pitch) {
    auto& transform = a_Entity.GetComponent<Transform>();
    auto& settings = a_Entity.GetComponent<Settings>();
    settings.pitch = a_Pitch;
    transform.rotation = GetRotationFromYPR(settings.yaw, settings.pitch, settings.roll);
}
template<typename EntityRefType>
void SetRoll(const EntityRefType& a_Entity, float a_Roll) {
    auto& transform = a_Entity.GetComponent<Transform>();
    auto& settings = a_Entity.GetComponent<Settings>();
    settings.roll = a_Roll;
    transform.rotation = GetRotationFromYPR(settings.yaw, settings.pitch, settings.roll);
}
}
