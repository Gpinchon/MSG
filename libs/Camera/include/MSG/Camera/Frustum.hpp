#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Plane.hpp>

#include <array>

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
enum class CameraFrustumFace {
    Left,
    Right,
    Bottom,
    Top,
    Near,
    Far,
    MaxValue
};
class CameraFrustum : public std::array<Plane, int(CameraFrustumFace::MaxValue)> {
public:
    using std::array<Plane, int(CameraFrustumFace::MaxValue)>::array;
    using std::array<Plane, int(CameraFrustumFace::MaxValue)>::operator[];
    auto& operator[](const CameraFrustumFace& a_Face) const noexcept { return operator[](size_t(a_Face)); }
    auto& operator[](const CameraFrustumFace& a_Face) noexcept { return operator[](size_t(a_Face)); }
};
}