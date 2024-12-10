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
#include <SG/Component/Transform.hpp>
#include <SG/Core/Frustum.hpp>
#include <SG/Core/Shapes/Plane.hpp>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <variant>

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
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
    Projection()
        : Projection(PerspectiveInfinite())
    {
    }
    Projection(const Projection&) = default;
    Projection(PerspectiveInfinite data);
    Projection(Perspective data);
    Projection(Orthographic data);
    Frustum GetFrustum(const Transform& a_CameraTransform = {}) const;
    template <typename T>
    inline auto& Get() const
    {
        return std::get<T>(_data);
    }
    inline auto GetMatrix() const
    {
        return _matrix;
    }
    inline operator glm::mat4() const
    {
        return GetMatrix();
    }
    inline glm::mat4 operator*(const glm::mat4& other) const
    {
        return GetMatrix() * other;
    }
    inline glm::mat4 operator*(const Projection& other) const
    {
        return GetMatrix() * other.GetMatrix();
    }

private:
    std::variant<PerspectiveInfinite, Perspective, Orthographic> _data;
    glm::mat4x4 _matrix;
};

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
    , _matrix(glm::ortho(data.xmag, data.xmag, data.ymag, data.ymag, data.znear, data.zfar))
{
}

inline Frustum GetOrthoFrustum(
    const Transform& a_CameraTransform,
    const Projection::Orthographic& a_Persp)
{
    // TODO determine if this is really needed
    return {};
}

inline Frustum Projection::GetFrustum(const Transform& a_CameraTransform) const
{
    auto viewMatrix = glm::inverse(a_CameraTransform.GetWorldTransformMatrix());
    auto m          = GetMatrix() * viewMatrix;
    Frustum frustum;
    frustum[FrustumFace::Left]   = glm::row(m, 3) + glm::row(m, 0);
    frustum[FrustumFace::Right]  = glm::row(m, 3) - glm::row(m, 0);
    frustum[FrustumFace::Bottom] = glm::row(m, 3) + glm::row(m, 1);
    frustum[FrustumFace::Top]    = glm::row(m, 3) - glm::row(m, 1);
    frustum[FrustumFace::Near]   = glm::row(m, 3) + glm::row(m, 2);
    if (type == ProjectionType::PerspectiveInfinite) {
        frustum[FrustumFace::Far] = Plane(
            -frustum[FrustumFace::Near].GetNormal(),
            std::numeric_limits<float>::max());
    } else {
        frustum[FrustumFace::Far] = glm::row(m, 3) - glm::row(m, 2);
    }
    for (auto& plane : frustum)
        plane.Normalize();
    return frustum;
}
}
