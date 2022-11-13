#pragma once

#include <Tools/Halton.hpp>

namespace TabGraph::Tools {
template<size_t Samples, size_t Bands>
inline constexpr SphericalHarmonics<Samples, Bands>::Sample::Sample(const size_t a_X, const size_t a_Y) {
    constexpr auto epsilon = std::numeric_limits<double>::epsilon();
    const auto index = (a_X * Samples) + a_Y;
    const auto randI = index % 256;
    constexpr auto halton2 = Halton<2>::Sequence<256>();
    constexpr auto halton3 = Halton<3>::Sequence<256>();
    
    const auto randX = (halton2[randI] * 2.0 - 1.0) * SampleRcp;
    const auto x = std::clamp((a_X + randX) * SampleRcp, 0.0, 1.0);
    constexpr auto thetaE = gcem::acos(2.0 * epsilon - 1.0); //lowest value allowed
    theta = x > 0 ? gcem::acos(2.0 * x - 1.0) : thetaE;
    //2.0 * gcem::acos(gcem::sqrt(1.0 - x)); //alternative form, more accurate but slower

    const auto randY = (halton3[randI] * 2.0 - 1.0) * SampleRcp;
    const auto y = std::clamp((a_Y + randY) * SampleRcp, 0.0, 1.0);
    constexpr auto phiE = 2.0 * M_PI * epsilon; //lowest value allowed
    phi = y > 0 ? 2.0 * M_PI * y : phiE;

    const auto sinTheta = gcem::sin(theta);
    const auto cosTheta = gcem::cos(theta);
    const auto sinPhi = gcem::sin(phi);
    const auto cosPhi = gcem::cos(phi);
    vec = {
        sinTheta * cosPhi,
        sinTheta * sinPhi,
        cosTheta
    };

    for (auto l = 0; l < Bands; ++l) {
        for (auto m = -l; m <= l; ++m) {
            const auto index = l * (l + 1) + m;
            _coeffs[index] = SHCoeff(l, m, vec, theta, phi);
        }
    }
}
}