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
#include <SG/Component/Camera.hpp>

#include <SG/Entity/Node.hpp>

#include <SG/Common.hpp>

#include <array>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/vec3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG::SG::Camera {
#define CAMERA_COMPONENTS NODE_COMPONENTS, SG::Component::Camera
/** @return the total nbr of Cameras created since start-up */
uint32_t& GetNbr();
template <typename RegistryType>
auto Create(const RegistryType& a_Registry)
{
    auto entity                                         = SG::Node::Create(a_Registry);
    entity.template GetComponent<SG::Component::Name>() = "CameraRoot_" + std::to_string(++GetNbr());
    entity.template AddComponent<SG::Component::Camera>();
    entity.template GetComponent<SG::Component::Camera>().name = "Camera_" + std::to_string(GetNbr());
    return entity;
}

/**
 * @brief alias for inverse TransformMatrix
 * @return the camera's view matrix
 */
template <typename EntityRefType>
auto GetViewMatrix(const EntityRefType& a_Entity)
{
    auto transform = a_Entity.template GetComponent<SG::Component::Transform>().GetWorldTransformMatrix();
    return glm::inverse(transform);
}

template <typename EntityRefType>
auto ExtractFrustum(const EntityRefType& a_Entity, const Component::Projection& a_Projection)
{
    std::array<glm::vec3, 8> NDCCube {
        glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f)
    };
    auto invVP = glm::inverse(a_Projection * GetViewMatrix(a_Entity));
    for (auto& v : NDCCube) {
        glm::vec4 normalizedCoord = invVP * glm::vec4(v, 1);
        v                         = glm::vec3(normalizedCoord) / normalizedCoord.w;
    }
    return NDCCube;
}

/**
 * @brief Computes the camera frustum's 8 corners
 * @return the camera frustum's 8 corners in world space
 */
template <typename EntityRefType>
auto ExtractFrustum(const EntityRefType& a_Entity)
{
    return ExtractFrustum(a_Entity, a_Entity.template GetComponent<Component::Projection>());
}

}
