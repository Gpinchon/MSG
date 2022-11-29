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
namespace TabGraph::SG {
#define CAMERA_COMPONENTS NODE_COMPONENTS, SG::CameraProjection
/** @return the total nbr of Cameras created since start-up */
uint32_t& GetCameraNbr();
template<typename RegistryType>
auto CreateCamera(const RegistryType& a_Registry) {
    auto entity = SG::CreateNode(a_Registry);
    entity.GetComponent<SG::Name>() = "Camera_" + std::to_string(++GetCameraNbr());
    entity.AddComponent<SG::CameraProjection>();
    return entity;
}

/**
* @brief The default "general purpose" camera
*/
class Camera : public Inherit<Node, Camera> {
public:
    Camera(CameraProjection = CameraProjection::PerspectiveInfinite());
    Camera(const std::string& name, CameraProjection = CameraProjection::PerspectiveInfinite());
    ~Camera() = default;

    /**
        * @brief alias for inverse TransformMatrix
        * @return the camera's view matrix
        */
    glm::mat4 GetViewMatrix();
    /**
        * @brief Computes the camera frustum's 8 corners
        * @return the camera frustum's 8 corners in world space
    */
    std::array<glm::vec3, 8> ExtractFrustum();

    inline const CameraProjection& GetProjection() const {
        return _projection;
    }
    inline void SetProjection(const CameraProjection& projectionType) {
        _projection = projectionType;
    }

private:
    CameraProjection _projection;
};
}
