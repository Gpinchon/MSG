#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Shapes/Plane.hpp>

#include <array>

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
enum class FrustumFace {
    Left,
    Right,
    Bottom,
    Top,
    Near,
    Far,
    MaxValue
};
class Frustum : public std::array<Plane, int(FrustumFace::MaxValue)> {
public:
    using std::array<Plane, int(FrustumFace::MaxValue)>::array;
    using std::array<Plane, int(FrustumFace::MaxValue)>::operator[];
    auto& operator[](const FrustumFace& a_Face) const noexcept { return operator[](size_t(a_Face)); }
    auto& operator[](const FrustumFace& a_Face) noexcept { return operator[](size_t(a_Face)); }
};
}