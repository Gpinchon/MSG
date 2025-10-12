#include <MSG/Camera/Frustum.hpp>
#include <MSG/Camera/Projection.hpp>
#include <MSG/Plane.hpp>
#include <MSG/Transform.hpp>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <variant>

Msg::CameraProjection::CameraProjection()
    : CameraProjection(CameraProjectionPerspectiveInfinite())
{
}

Msg::CameraProjection::CameraProjection(CameraProjectionPerspectiveInfinite data)
    : CameraProjectionBase(data)
    , _matrix(glm::infinitePerspective(glm::radians(data.fov), data.aspectRatio, data.znear))
{
}

Msg::CameraProjection::CameraProjection(CameraProjectionPerspective data)
    : CameraProjectionBase(data)
    , _matrix(glm::perspective(glm::radians(data.fov), data.aspectRatio, data.znear, data.zfar))
{
}

Msg::CameraProjection::CameraProjection(CameraProjectionOrthographic data)
    : CameraProjectionBase(data)
    , _matrix(glm::ortho(data.left, data.right, data.bottom, data.top, data.znear, data.zfar))
{
}

void Msg::CameraProjection::UpdateMatrix()
{
    std::visit([this](auto& a_Data) { *this = a_Data; }, *this);
}

float Msg::CameraProjection::GetZNear() const
{
    return std::visit([](auto& a_Proj) { return a_Proj.znear; }, *this);
}

template <typename T>
float GetZFar(const T& a_Proj)
{
    return a_Proj.zfar;
}

template <>
float GetZFar(const Msg::CameraProjectionPerspectiveInfinite&)
{
    return std::numeric_limits<float>::infinity();
}

float Msg::CameraProjection::GetZFar() const
{
    return std::visit([](auto& a_Proj) { return ::GetZFar(a_Proj); }, *this);
}

Msg::CameraFrustum GetOrthoFrustum(
    const Msg::Transform& a_CameraTransform,
    const Msg::CameraProjectionOrthographic& a_Persp)
{
    // TODO determine if this is really needed
    return {};
}

Msg::CameraFrustum Msg::CameraProjection::GetFrustum(const Transform& a_CameraTransform) const
{
    auto viewMatrix = glm::inverse(a_CameraTransform.GetWorldTransformMatrix());
    auto m          = GetMatrix() * viewMatrix;
    CameraFrustum frustum;
    frustum[CameraFrustumFace::Left]   = glm::row(m, 3) + glm::row(m, 0);
    frustum[CameraFrustumFace::Right]  = glm::row(m, 3) - glm::row(m, 0);
    frustum[CameraFrustumFace::Bottom] = glm::row(m, 3) + glm::row(m, 1);
    frustum[CameraFrustumFace::Top]    = glm::row(m, 3) - glm::row(m, 1);
    frustum[CameraFrustumFace::Near]   = glm::row(m, 3) + glm::row(m, 2);
    if (GetType() == CameraProjectionType::PerspectiveInfinite) {
        frustum[CameraFrustumFace::Far] = Plane(
            -frustum[CameraFrustumFace::Near].GetNormal(),
            std::numeric_limits<float>::max());
    } else {
        frustum[CameraFrustumFace::Far] = glm::row(m, 3) - glm::row(m, 2);
    }
    for (auto& plane : frustum)
        plane.Normalize();
    return frustum;
}
