/*
* @Author: gpinchon
* @Date:   2019-07-16 08:53:02
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
namespace TabGraph::SG::OrbitCamera {
struct Settings {
    ECS::DefaultRegistry::EntityRefType target;
    float phi{ 0 };
    float theta{ 0 };
    float radius{ 0 };
};

#define ORBITCAMERA_COMPONENTS CAMERA_COMPONENTS, SG::OrbitCamera::Settings

/** @return the total nbr of FPS Cameras created since start-up */
uint32_t& GetNbr();
template<typename RegistryType>
auto Create(const RegistryType& a_Registry) {
    auto entity = SG::Camera::Create(a_Registry);
    entity.GetComponent<SG::Name>() = "FPSCamera_" + std::to_string(++GetNbr());
    entity.AddComponent<Settings>();
    return entity;
}

template<typename EntityRefType>
void Update(const EntityRefType& a_Entity) {
    auto& settings = a_Entity.GetComponent<Settings>();
    glm::vec3 targetPosition(0);
    if (settings.target)
        targetPosition = Target()->GetWorldPosition();
    //TODO Add SetWorldPosition to Node
    SetLocalPosition(targetPosition + GetRadius() * glm::vec3(sin(GetPhi()) * cos(GetTheta()), sin(GetPhi()) * sin(GetTheta()), cos(GetPhi())));
    LookAt(targetPosition);
}
}
