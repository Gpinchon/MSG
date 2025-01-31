#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Camera/Frustum.hpp>
#include <MSG/Plane.hpp>
#include <MSG/Transform.hpp>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <variant>

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
enum class CameraProjectionType {
    PerspectiveInfinite,
    Perspective,
    Orthographic,
    MaxValue
};
class CameraProjection {
public:
    CameraProjectionType type { CameraProjectionType::PerspectiveInfinite };
    struct PerspectiveInfinite {
        float fov { 45 };
        float aspectRatio { 16 / 9.f };
        float znear { 0.1f };
    };
    struct Perspective : PerspectiveInfinite {
        float zfar { 1000 };
    };
    struct Orthographic {
        float xmag { 50 };
        float ymag { 50 };
        float znear { 0.1f };
        float zfar { 1000 };
    };
    CameraProjection();
    CameraProjection(const CameraProjection&) = default;
    CameraProjection(PerspectiveInfinite data);
    CameraProjection(Perspective data);
    CameraProjection(Orthographic data);
    CameraFrustum GetFrustum(const Transform& a_CameraTransform = {}) const;
    template <typename T>
    inline const T& Get() const;
    inline const glm::mat4x4& GetMatrix() const;
    inline operator const glm::mat4&() const;
    inline glm::mat4 operator*(const glm::mat4& other) const;
    inline glm::mat4 operator*(const CameraProjection& other) const;

private:
    glm::mat4x4 _matrix;
    std::variant<PerspectiveInfinite, Perspective, Orthographic> _data;
};

inline CameraProjection::CameraProjection()
    : CameraProjection(PerspectiveInfinite())
{
}

inline CameraProjection::CameraProjection(PerspectiveInfinite data)
    : type(CameraProjectionType::PerspectiveInfinite)
    , _data(data)
    , _matrix(glm::infinitePerspective(glm::radians(data.fov), data.aspectRatio, data.znear))
{
}

inline CameraProjection::CameraProjection(Perspective data)
    : type(CameraProjectionType::Perspective)
    , _data(data)
    , _matrix(glm::perspective(glm::radians(data.fov), data.aspectRatio, data.znear, data.zfar))
{
}

inline CameraProjection::CameraProjection(Orthographic data)
    : type(CameraProjectionType::Orthographic)
    , _data(data)
    , _matrix(glm::ortho(-data.xmag, data.xmag, -data.ymag, data.ymag, data.znear, data.zfar))
{
}

inline CameraFrustum GetOrthoFrustum(
    const Transform& a_CameraTransform,
    const CameraProjection::Orthographic& a_Persp)
{
    // TODO determine if this is really needed
    return {};
}

inline CameraFrustum CameraProjection::GetFrustum(const Transform& a_CameraTransform) const
{
    auto viewMatrix = glm::inverse(a_CameraTransform.GetWorldTransformMatrix());
    auto m          = GetMatrix() * viewMatrix;
    CameraFrustum frustum;
    frustum[CameraFrustumFace::Left]   = glm::row(m, 3) + glm::row(m, 0);
    frustum[CameraFrustumFace::Right]  = glm::row(m, 3) - glm::row(m, 0);
    frustum[CameraFrustumFace::Bottom] = glm::row(m, 3) + glm::row(m, 1);
    frustum[CameraFrustumFace::Top]    = glm::row(m, 3) - glm::row(m, 1);
    frustum[CameraFrustumFace::Near]   = glm::row(m, 3) + glm::row(m, 2);
    if (type == CameraProjectionType::PerspectiveInfinite) {
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

inline const glm::mat4x4& CameraProjection::GetMatrix() const
{
    return _matrix;
}

inline CameraProjection::operator const glm::mat4&() const
{
    return GetMatrix();
}

inline glm::mat4 CameraProjection::operator*(const glm::mat4& other) const
{
    return GetMatrix() * other;
}

inline glm::mat4 CameraProjection::operator*(const CameraProjection& other) const
{
    return GetMatrix() * other.GetMatrix();
}

template <typename T>
inline const T& CameraProjection::Get() const
{
    return std::get<T>(_data);
}
}
