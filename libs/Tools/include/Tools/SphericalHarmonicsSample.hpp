#pragma once

#include <Tools/SphericalHarmonics.hpp>

namespace TabGraph::Tools {
template<size_t Samples, size_t Bands>
inline constexpr SphericalHarmonics<Samples, Bands>::Sample::Sample(const size_t a_X, const size_t a_Y)
{
    const auto index = (a_X * Samples) + a_Y;

    const auto randI = (index + 1) % 256;

    const auto randX = (Halton<2>::Value(randI) * 2.0 - 1.0) * SampleRcp;
    const auto x{ std::clamp((a_X + randX) * SampleRcp, 0.0, 1.0) };
    theta = gcem::acos(2.0 * x - 1.0);

    const auto randY = (Halton<3>::Value(randI) * 2.0 - 1.0) * SampleRcp;
    const auto y{ std::clamp((a_Y + randY) * SampleRcp, 0.0, 1.0) };
    phi = 2.0 * M_PI * y;

    const auto sinTheta{ gcem::sin(theta) };
    const auto cosTheta{ gcem::cos(theta) };
    const auto sinPhi{ gcem::sin(phi) };
    const auto cosPhi{ gcem::cos(phi) };
    vec = glm::vec3{
        sinTheta * cosPhi,
        sinTheta * sinPhi,
        cosTheta
    };

    for (auto l{ 0 }; l < Bands; ++l) {
        for (int m{ -l }; m <= l; ++m) {
            const auto index{ l * (l + 1) + m };
            _coeffs[index] = SHCoeff(l, m, theta, phi);
        }
    }
}
}
