/*
 * @Author: gpinchon
 * @Date:   2021-07-21 21:53:09
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-07-21 22:04:50
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Transform.hpp>
#include <Core/Frustum.hpp>
#include <Core/Shapes/Plane.hpp>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <variant>

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Core {
enum class ProjectionType {
    PerspectiveInfinite,
    Perspective,
    Orthographic,
    MaxValue
};
class Projection {
public:
    ProjectionType type { ProjectionType::PerspectiveInfinite };
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
    Projection();
    Projection(const Projection&) = default;
    Projection(PerspectiveInfinite data);
    Projection(Perspective data);
    Projection(Orthographic data);
    Core::Frustum GetFrustum(const Transform& a_CameraTransform = {}) const;
    template <typename T>
    inline const T& Get() const;
    inline const glm::mat4x4& GetMatrix() const;
    inline operator const glm::mat4&() const;
    inline glm::mat4 operator*(const glm::mat4& other) const;
    inline glm::mat4 operator*(const Projection& other) const;

private:
    glm::mat4x4 _matrix;
    std::variant<PerspectiveInfinite, Perspective, Orthographic> _data;
};

inline Projection::Projection()
    : Projection(PerspectiveInfinite())
{
}

inline Projection::Projection(PerspectiveInfinite data)
    : type(ProjectionType::PerspectiveInfinite)
    , _data(data)
    , _matrix(glm::infinitePerspective(glm::radians(data.fov), data.aspectRatio, data.znear))
{
}

inline Projection::Projection(Perspective data)
    : type(ProjectionType::Perspective)
    , _data(data)
    , _matrix(glm::perspective(glm::radians(data.fov), data.aspectRatio, data.znear, data.zfar))
{
}

inline Projection::Projection(Orthographic data)
    : type(ProjectionType::Orthographic)
    , _data(data)
    , _matrix(glm::ortho(-data.xmag, data.xmag, -data.ymag, data.ymag, data.znear, data.zfar))
{
}

inline Core::Frustum GetOrthoFrustum(
    const Transform& a_CameraTransform,
    const Projection::Orthographic& a_Persp)
{
    // TODO determine if this is really needed
    return {};
}

inline Core::Frustum Projection::GetFrustum(const Transform& a_CameraTransform) const
{
    auto viewMatrix = glm::inverse(a_CameraTransform.GetWorldTransformMatrix());
    auto m          = GetMatrix() * viewMatrix;
    Core::Frustum frustum;
    frustum[Core::FrustumFace::Left]   = glm::row(m, 3) + glm::row(m, 0);
    frustum[Core::FrustumFace::Right]  = glm::row(m, 3) - glm::row(m, 0);
    frustum[Core::FrustumFace::Bottom] = glm::row(m, 3) + glm::row(m, 1);
    frustum[Core::FrustumFace::Top]    = glm::row(m, 3) - glm::row(m, 1);
    frustum[Core::FrustumFace::Near]   = glm::row(m, 3) + glm::row(m, 2);
    if (type == ProjectionType::PerspectiveInfinite) {
        frustum[Core::FrustumFace::Far] = Core::Plane(
            -frustum[Core::FrustumFace::Near].GetNormal(),
            std::numeric_limits<float>::max());
    } else {
        frustum[Core::FrustumFace::Far] = glm::row(m, 3) - glm::row(m, 2);
    }
    for (auto& plane : frustum)
        plane.Normalize();
    return frustum;
}

inline const glm::mat4x4& Projection::GetMatrix() const
{
    return _matrix;
}

inline Projection::operator const glm::mat4&() const
{
    return GetMatrix();
}

inline glm::mat4 Projection::operator*(const glm::mat4& other) const
{
    return GetMatrix() * other;
}

inline glm::mat4 Projection::operator*(const Projection& other) const
{
    return GetMatrix() * other.GetMatrix();
}

template <typename T>
inline const T& Projection::Get() const
{
    return std::get<T>(_data);
}
}
