/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-21 21:54:14
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Camera/Projection.hpp>
#include <SG/Common.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Node/Node.hpp>

#include <array>
#include <functional>
#include <glm/vec3.hpp>
#include <string>
#include <variant>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Camera {
#define CAMERA_COMPONENTS NODE_COMPONENTS, SG::CameraProjection
/** @return the total nbr of Cameras created since start-up */
uint32_t& GetNbr();
template<typename RegistryType>
auto Create(const RegistryType& a_Registry) {
    auto entity = SG::CreateNode(a_Registry);
    entity.GetComponent<SG::Name>() = "Camera_" + std::to_string(++GetNbr());
    entity.AddComponent<SG::CameraProjection>();
    return entity;
}

/**
* @brief alias for inverse TransformMatrix
* @return the camera's view matrix
*/
template<typename EntityRefType>
auto GetViewMatrix(const EntityRefType& a_Entity) {
    return glm::inverse(NodeGetWorldTransformMatrix(a_Entity));
}
/**
* @brief Computes the camera frustum's 8 corners
* @return the camera frustum's 8 corners in world space
*/
template<typename EntityRefType>
auto ExtractFrustum(const EntityRefType& a_Entity) {
    static std::array<glm::vec3, 8> NDCCube{
        glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f)
    };
    auto invVP = glm::inverse(a_Entity.GetComponent<CameraProjection>() * GetViewMatrix(a_Entity));
    for (auto& v : NDCCube) {
        glm::vec4 normalizedCoord = invVP * glm::vec4(v, 1);
        v = glm::vec3(normalizedCoord) / normalizedCoord.w;
    }
    return NDCCube;
}
}
