/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-21 21:59:34
*/

#include <SG/Camera/Camera.hpp>

namespace TabGraph::SG {
Camera::Camera(CameraProjection projection)
    : Inherit()
{
    static auto s_CameraNbr = 0u;
    SetName("Camera" + std::to_string(s_CameraNbr));
    SetProjection(projection);
    ++s_CameraNbr;
}

Camera::Camera(const std::string& name, CameraProjection projection)
    : Camera(projection)
{
    SetName(name);
    SetProjection(projection);
}

std::array<glm::vec3, 8> Camera::ExtractFrustum()
{
    static std::array<glm::vec3, 8> NDCCube {
        glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f)
    };
    auto invVP = glm::inverse(GetProjection() * GetViewMatrix());
    for (auto& v : NDCCube) {
        glm::vec4 normalizedCoord = invVP * glm::vec4(v, 1);
        v = glm::vec3(normalizedCoord) / normalizedCoord.w;
    }
    return NDCCube;
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::inverse(GetWorldTransformMatrix());
}

}
