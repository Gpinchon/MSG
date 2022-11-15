#pragma once

#include <Tools/Halton.hpp>
#include <Tools/Factorial.hpp>
#include <Tools/LegendrePolynomial.hpp>

#include <algorithm>

namespace TabGraph::Tools {
constexpr double K(int l, int m)
{
    //normalization constant
    //b == bands
    //l = [ 0..b]
    //m = [-l..l]
    if (__builtin_is_constant_evaluated()) {
        const double kml = (2.0 * l + 1) * Factorial(l - gcem::abs(m)) / (4.0 * M_PI * Factorial(l + gcem::abs(m)));
        return gcem::sqrt(kml);
    }
    else {
        const double kml = (2.0 * l + 1) * Factorial(l - std::abs(m)) / (4.0 * M_PI * Factorial(l + std::abs(m)));
        return sqrt(kml);
    }
}

constexpr auto ComputeSHCoeff(int32_t l, int32_t m, double theta, double phi)
{
    //theta = [0..PI]
    //phi   = [0..2*PI]
    constexpr auto sqrt2 = gcem::sqrt(2.0);
    const auto kml = K(l, m);
    if (__builtin_is_constant_evaluated()) {
        if (m > 0) return sqrt2 * kml * gcem::cos(m * phi) * LegendrePolynomial(l, m, gcem::cos(theta));
        else if (m < 0) return sqrt2 * kml * gcem::sin(-m * phi) * LegendrePolynomial(l, -m, gcem::cos(theta));
        else return kml * LegendrePolynomial(l, 0, gcem::cos(theta));
    }
    else {
        if (m > 0) return sqrt2 * kml * std::cos(m * phi) * LegendrePolynomial(l, m, std::cos(theta));
        else if (m < 0) return sqrt2 * kml * std::sin(-m * phi) * LegendrePolynomial(l, -m, std::cos(theta));
        else return kml * LegendrePolynomial(l, 0, std::cos(theta));
    }
}

constexpr double SHCoeff(int32_t l, int32_t m, const glm::vec3& a_Vec, double theta, double phi)
{
    const glm::vec3 N{ a_Vec };
    const glm::vec3 N2{ N * N };
    switch (l) {
    case 0:
        return 0.282095;
    case 1:
        switch (m) {
        case -1:
            return -0.488603 * N.y;
        case 0:
            return 0.488603 * N.z;
        case 1:
            return -0.488603 * N.x;
        }
    case 2:
        switch (m) {
        case -2:
            return 1.092548 * N.x * N.y;
        case -1:
            return -1.092548 * N.y * N.z;
        case 0:
            return 0.315392 * (-N2.x - N2.y + 2.0 * N2.z);
        case 1:
            return -1.092548 * N.x * N.z;
        case 2:
            return 0.546274 * (N2.x - N2.y);
        }
    case 3:
        switch (m) {
        case -3:
            return -0.590044 * N.y * (3 * N2.x - N2.y);
        case -2:
            return 2.890611 * N.x * N.y * N.z;
        case -1:
            return -0.457046 * N.y * (4 * N2.z - N2.x - N2.y);
        case 0:
            return 0.373176 * N.z * (2 * N2.z - 3 * N2.x - 3 * N2.y);
        case 1:
            return -0.457046 * N.x * (4 * N2.z - N2.x - N2.y);
        case 2:
            return 1.445306 * N.z * (N2.x - N2.y);
        case 3:
            return -0.590044 * N.x * (N2.x - 3 * N2.y);
        }
    }
    return ComputeSHCoeff(l, m, theta, phi);
}

constexpr double SHCoeff(int32_t l, int32_t m, double theta, double phi)
{
    double sinTheta = 0;
    double cosTheta = 0;
    double sinPhi = 0;
    double cosPhi = 0;
    if (__builtin_is_constant_evaluated()) {
        sinTheta = gcem::sin(theta);
        cosTheta = gcem::cos(theta);
        sinPhi = gcem::sin(phi);
        cosPhi = gcem::cos(phi);
    }
    else {
        sinTheta = std::sin(theta);
        cosTheta = std::cos(theta);
        sinPhi = std::sin(phi);
        cosPhi = std::cos(phi);
    }
    return SHCoeff(l, m, {
            sinTheta * cosPhi,
            sinTheta * sinPhi,
            cosTheta
        }, theta, phi);
}

constexpr double SHCoeff(int32_t l, int32_t m, const glm::vec3& a_Vec)
{
    const glm::vec3 N{ a_Vec };
    const glm::vec3 N2{ N * N };
    switch (l) {
    case 0:
        return 0.282095;
    case 1:
        switch (m) {
        case -1:
            return -0.488603 * N.y;
        case 0:
            return 0.488603 * N.z;
        case 1:
            return -0.488603 * N.x;
        }
    case 2:
        switch (m) {
        case -2:
            return 1.092548 * N.x * N.y;
        case -1:
            return -1.092548 * N.y * N.z;
        case 0:
            return 0.315392 * (-N2.x - N2.y + 2.0 * N2.z);
        case 1:
            return -1.092548 * N.x * N.z;
        case 2:
            return 0.546274 * (N2.x - N2.y);
        }
    case 3:
        switch (m) {
        case -3:
            return -0.590044 * N.y * (3 * N2.x - N2.y);
        case -2:
            return 2.890611 * N.x * N.y * N.z;
        case -1:
            return -0.457046 * N.y * (4 * N2.z - N2.x - N2.y);
        case 0:
            return 0.373176 * N.z * (2 * N2.z - 3 * N2.x - 3 * N2.y);
        case 1:
            return -0.457046 * N.x * (4 * N2.z - N2.x - N2.y);
        case 2:
            return 1.445306 * N.z * (N2.x - N2.y);
        case 3:
            return -0.590044 * N.x * (N2.x - 3 * N2.y);
        }
    }
    if (__builtin_is_constant_evaluated()) {
        const auto theta = gcem::atan(gcem::sqrt(N.x * N.x + N.y * N.y) / N.z);
        const auto phi = gcem::atan(N.y / N.x);
        return ComputeSHCoeff(l, m, theta, phi);
    }
    else {
        const auto theta = std::atan(std::sqrt(N.x * N.x + N.y * N.y) / N.z);
        const auto phi = std::atan(N.y / N.x);
        return ComputeSHCoeff(l, m, theta, phi);
    }
}

constexpr double SHCoeff(int32_t i, double theta, double phi)
{
    int l = 0;
    if (__builtin_is_constant_evaluated()) l = int(gcem::sqrt(i));
    else l = int(std::sqrt(i));
    const auto m = -l + i - (l * l);
    return SHCoeff(l, m, theta, phi);
}

constexpr double SHCoeff(int32_t i, const glm::vec3& a_Vec)
{
    int l = 0;
    if (__builtin_is_constant_evaluated()) l = int(gcem::sqrt(i));
    else l = int(std::sqrt(i));
    const auto m = -l + i - (l * l);
    return SHCoeff(l, m, a_Vec);
}

template<size_t Samples, size_t Bands>
inline constexpr SphericalHarmonics<Samples, Bands>::Sample::Sample(const size_t a_X, const size_t a_Y) {
    constexpr auto epsilon = std::numeric_limits<double>::epsilon();
    constexpr auto halton2 = Halton<2>::Sequence<256>();
    constexpr auto halton3 = Halton<3>::Sequence<256>();
    const auto index = (a_X * Samples) + a_Y;
    const auto randI = index % 256;
    
    const auto randX = (halton2[randI] * 2.0 - 1.0) * SampleRcp;
    const auto x = std::clamp((a_X + randX) * SampleRcp, 0.0, 1.0);
    constexpr auto thetaE = gcem::acos(2.0 * epsilon - 1.0); //lowest value allowed
    if (__builtin_is_constant_evaluated())
        theta = x > 0 ? gcem::acos(2.0 * x - 1.0) : thetaE;
    else theta = x > 0 ? std::acos(2.0 * x - 1.0) : thetaE;
    //2.0 * gcem::acos(gcem::sqrt(1.0 - x)); //alternative form, more accurate but slower

    const auto randY = (halton3[randI] * 2.0 - 1.0) * SampleRcp;
    const auto y = std::clamp((a_Y + randY) * SampleRcp, 0.0, 1.0);
    constexpr auto phiE = 2.0 * M_PI * epsilon; //lowest value allowed
    phi = y > 0 ? 2.0 * M_PI * y : phiE;

    double sinTheta = 0;
    double cosTheta = 0;
    double sinPhi = 0;
    double cosPhi = 0;
    if (__builtin_is_constant_evaluated()) {
        sinTheta = gcem::sin(theta);
        cosTheta = gcem::cos(theta);
        sinPhi = gcem::sin(phi);
        cosPhi = gcem::cos(phi);
    }
    else {
        sinTheta = std::sin(theta);
        cosTheta = std::cos(theta);
        sinPhi = std::sin(phi);
        cosPhi = std::cos(phi);
    }

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