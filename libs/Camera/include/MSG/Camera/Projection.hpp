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
    Orthographic
};
struct CameraProjectionPerspectiveInfinite {
    float fov { 45 }; // FOV in degrees
    float aspectRatio { 16 / 9.f };
    float znear { 0.1f };
};
struct CameraProjectionPerspective : CameraProjectionPerspectiveInfinite {
    float zfar { 1000 };
};
struct CameraProjectionOrthographic {
    float xmag { 50 };
    float ymag { 50 };
    float znear { 0.1f };
    float zfar { 1000 };
};
using CameraProjectionBase = std::variant<CameraProjectionPerspectiveInfinite, CameraProjectionPerspective, CameraProjectionOrthographic>;
class CameraProjection : public CameraProjectionBase {
public:
    CameraProjection();
    CameraProjection(const CameraProjection&) = default;
    CameraProjection(CameraProjectionPerspectiveInfinite data);
    CameraProjection(CameraProjectionPerspective data);
    CameraProjection(CameraProjectionOrthographic data);
    CameraFrustum GetFrustum(const Transform& a_CameraTransform = {}) const;
    /** @brief recomputes the matrix */
    void UpdateMatrix();
    /** @brief helper function to get the inner projection zNear */
    float GetZNear() const;
    /** @brief helper function to get the inner projection zFar */
    float GetZFar() const;
    inline CameraProjectionType GetType() const { return CameraProjectionType(index()); }
    inline const glm::mat4x4& GetMatrix() const { return _matrix; }
    inline operator const glm::mat4&() const { return GetMatrix(); }
    inline glm::mat4 operator*(const glm::mat4& other) const { return GetMatrix() * other; }
    inline glm::mat4 operator*(const CameraProjection& other) const { return GetMatrix() * other.GetMatrix(); }

private:
    glm::mat4x4 _matrix;
};
}
