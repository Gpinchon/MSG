#pragma once

namespace TabGraph::Tools {
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