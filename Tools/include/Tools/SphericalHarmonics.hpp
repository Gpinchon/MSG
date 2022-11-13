/*
* @Author: gpinchon
* @Date:   2021-03-11 12:22:36
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-11 12:28:04
*/
#pragma once

#include <glm/glm.hpp>
#include <gcem.hpp>

#include <array>
#include <vector>
#include <functional>
#include <algorithm>

namespace TabGraph::Tools {
/**
* @brief Computes Spherical Harmonics coefficients from spherical coordinates
*/
constexpr double SHCoeff(int32_t l, int32_t m, double theta, double phi);
/**
* @brief Computes Spherical Harmonics coefficients from cartesian coordinates
* The first 4 bands are precomputed and will be faster to get (except for static calculations)
*/
constexpr double SHCoeff(int l, int m, const glm::vec3& a_Vec);
/**
* @brief Computes Spherical Harmonics coefficients from cartesian and spherical coordinates
* The first 4 bands are precomputed and will be faster to get (except for static calculations)
*/
constexpr double SHCoeff(int32_t l, int32_t m, const glm::vec3& a_Vec, double theta, double phi);

/**
* @brief Constructs a Spherical Harmonics with specified samples and bands
* @param Samples : the number of samples in latitude & longitude, total samples will be N * N
* @param Bands   : the number of bands in the final Spherical Harmonics, total coeffs will be N * N, default is 4
*/
template<size_t Samples, size_t Bands = 4>
class SphericalHarmonics {
public:
    static constexpr auto SampleCount = Samples * Samples;
    static constexpr auto SampleRcp = 1 / double(Samples);
    /**
    * @brief Each sample generates a list of coefficients in relation with its latitude & longitude
    */
    struct Sample {
        static constexpr auto CoeffCount = Bands * Bands;
        constexpr Sample() = default;
        constexpr Sample(const size_t a_X, const size_t a_Y);
        double theta{ 0 }, phi{ 0 };
        glm::dvec3 vec{ 0 };
    private:
        friend SphericalHarmonics;
        std::array<double, CoeffCount> _coeffs{};
    };
    constexpr SphericalHarmonics() noexcept;
    template<typename R>
    constexpr auto ProjectFunction(std::function<R(const Sample&)>) const->std::array<R, Sample::CoeffCount>;
    template<template<size_t, size_t> class Op, typename R>
    constexpr auto ProjectFunction() const->std::array<R, Sample::CoeffCount>;

private:
    std::vector<Sample> _samples;
};
}

#include <Tools/SphericalHarmonicsSample.inl>
#include <Tools/SphericalHarmonics.inl>
