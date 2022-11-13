#pragma once

#include <Tools/Factorial.hpp>
#include <Tools/LegendrePolynomial.hpp>

namespace TabGraph::Tools {
constexpr double K(int l, int m)
{
    //normalization constant
    //b == bands
    //l = [ 0..b]
    //m = [-l..l]
    const double kml = (2.0 * l + 1) * Factorial(l - gcem::abs(m)) / (4.0 * M_PI * Factorial(l + gcem::abs(m)));
    return gcem::sqrt(kml);
}
constexpr double SHCoeff(int32_t l, int32_t m, double theta, double phi)
{
    //theta = [0..PI]
    //phi   = [0..2*PI]
    constexpr auto sqrt2 = gcem::sqrt(2.0);
    const auto kml = K(l, m);
    if (m > 0) {
        return sqrt2 * kml * gcem::cos(m * phi) * LegendrePolynomial(l, m, gcem::cos(theta));
    }
    else if (m < 0) {
        return sqrt2 * kml * gcem::sin(-m * phi) * LegendrePolynomial(l, -m, gcem::cos(theta));
    }
    else //m == 0
        return kml * LegendrePolynomial(l, 0, gcem::cos(theta));
}
constexpr double SHCoeff(int l, int m, const glm::vec3& a_Vec)
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
    const auto theta = gcem::atan(gcem::sqrt(N.x * N.x + N.y * N.y) / N.z);
    const auto phi   = gcem::atan(N.y / N.x);
    return SHCoeff(l, m, theta, phi);
}

constexpr double SHCoeff(int l, int m, const glm::vec3& a_Vec, double theta, double phi)
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
    return SHCoeff(l, m, theta, phi);
}

template<size_t Samples, size_t Bands>
inline constexpr SphericalHarmonics<Samples, Bands>::SphericalHarmonics() noexcept
    : _samples(SampleCount)
{
    for (size_t x = 0; x < Samples; ++x) {
        for (size_t y = 0; y < Samples; ++y) {
            const auto i = (x * Samples) + y;
            _samples[i] = Sample(x, y);
        }
    }
}

template<size_t Samples, size_t Bands>
template<typename R>
constexpr auto SphericalHarmonics<Samples, Bands>::ProjectFunction(std::function<R(const Sample&)>) const->std::array<R, Sample::CoeffCount> {
    std::array<R, Sample::CoeffCount> result;
    for (auto n = 0u; n < Sample::CoeffCount; ++n) result[n] = R(0);
    for (size_t x = 0; x < Samples; ++x) {
        for (size_t y = 0; y < Samples; ++y) {
            const auto i = (x * Samples) + y;
            const auto sample = _samples[i];
            const auto value{ a_Fn(sample) };
            for (auto n = 0u; n < Sample::CoeffCount; ++n) {
                result[n] += value * float(sample._coeffs[n]);
            }
        }
    }
    //weight function is the surface area of a sphere
    constexpr auto weight{ 4 * M_PI };
    constexpr auto factor{ weight / double(SampleCount) };
    for (auto& coeff : result)
        coeff *= factor;
    return result;
}

template<size_t Samples, size_t Bands>
template<template<size_t, size_t> class Op, typename R>
constexpr auto SphericalHarmonics<Samples, Bands>::ProjectFunction() const->std::array<R, Sample::CoeffCount> {
    constexpr Op<Samples, Bands> op{};
    std::array<R, Sample::CoeffCount> result;
    for (auto n = 0u; n < Sample::CoeffCount; ++n) result[n] = R(0);
    for (size_t x = 0; x < Samples; ++x) {
        for (size_t y = 0; y < Samples; ++y) {
            const auto i = (x * Samples) + y;
            const auto sample = _samples[i];
            const auto value{ op(sample) };
            for (auto n = 0u; n < Sample::CoeffCount; ++n) {
                result[n] += value * sample._coeffs[n];
            }
        }
    }
    //weight function is the surface area of a sphere
    constexpr auto weight{ 4 * M_PI };
    constexpr auto factor{ weight / double(SampleCount) };
    for (auto& coeff : result)
        coeff *= factor;
    return result;
}
}