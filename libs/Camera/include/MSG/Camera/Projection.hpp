#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Camera/Frustum.hpp>
#include <MSG/Transform.hpp>

#include <glm/mat4x4.hpp>
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
        float fov { 45 }; // FOV in degrees
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
    /** @brief visits the projection data with specified functor and update it */
    template <typename OP>
    void Visit(const OP& a_Op);
    template <typename T>
    inline const T& Get() const { return std::get<T>(_data); }
    inline const glm::mat4x4& GetMatrix() const { return _matrix; }
    inline operator const glm::mat4&() const { return GetMatrix(); }
    inline glm::mat4 operator*(const glm::mat4& other) const { return GetMatrix() * other; }
    inline glm::mat4 operator*(const CameraProjection& other) const { return GetMatrix() * other.GetMatrix(); }

private:
    glm::mat4x4 _matrix;
    std::variant<PerspectiveInfinite, Perspective, Orthographic> _data;
};

template <typename OP>
void CameraProjection::Visit(const OP& a_Op)
{
    std::visit(a_Op, _data);
    std::visit([this](auto& a_Data) { *this = a_Data; }, _data);
}
}
