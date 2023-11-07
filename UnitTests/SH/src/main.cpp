#include <Tools/SphericalHarmonics.hpp>
#include <Tools/ScopedTimer.hpp>

#include <iostream>
#include <chrono>

#include <gtest/gtest.h>

using namespace TabGraph;

constexpr auto testValue = glm::dvec3(0, 1, 0);
constexpr auto samplingX = 10;
constexpr auto samplingY = 10;
constexpr auto samplingZ = 10;
constexpr auto SHSamples = 100;
constexpr auto SHBands   = 4;

template<typename T>
constexpr auto SampleSH(const glm::dvec3& N, const T& SH)
{
    const auto N2 = N * N;
    glm::dvec3 v{ 0 };
    for (int i = 0; i < SH.size(); ++i) {
        v += SH[i] * Tools::SHCoeff(i, N);
    }
    return v;
}

template<template<size_t, size_t> class Op, size_t Samples, size_t Bands>
constexpr auto TestFunc(const Tools::SphericalHarmonics<Samples, Bands>& SH, const std::string& a_Name) {
    std::cout << "Test " << a_Name << '\n';
    constexpr Op<Samples, Bands> op{};
    std::array<glm::dvec3, Bands* Bands> SHProj;
    {
        const auto testChrono = Tools::ScopedTimer("SH Evaluation");
        SHProj = SH.template Eval<Op, glm::dvec3>();
    }
    size_t testCount = 0;
    size_t testPassed = 0;
    {
        const auto testChrono = Tools::ScopedTimer("SH Sampling");
        for (auto x = -samplingX; x <= samplingX; ++x) {
            for (auto y = -samplingY; y <= samplingY; ++y) {
                for (auto z = -samplingZ; z <= samplingZ; ++z) {
                    ++testCount;
                    typename Tools::SphericalHarmonics<Samples, Bands>::Sample sample;
                    sample.vec = glm::normalize(glm::dvec3(x, y, z));
                    const auto expected = op(sample);
                    const auto result = SampleSH(sample.vec, SHProj);
                    if (!Tools::feq(expected.x, result.x, 0.05)) continue;
                    if (!Tools::feq(expected.y, result.y, 0.05)) continue;
                    if (!Tools::feq(expected.z, result.z, 0.05)) continue;
                    ++testPassed;
                }
            }
        }
    }
    const auto successRate = (testPassed / double(testCount) * 100.0);
    const auto success = successRate >= 80;
    std::cout << "Success Rate : " << successRate << "% " << (success ? "[Passed]" : "[Failed]") << '\n';
    return success;
}

template<size_t Samples, size_t Bands>
struct DotVec
{
    constexpr auto operator()(const typename Tools::SphericalHarmonics<Samples, Bands>::Sample& a_Sample) const {
        return glm::dvec3(glm::dot(a_Sample.vec, testValue));
    }
};

template<size_t Samples, size_t Bands>
struct CrossVec
{
    constexpr auto operator()(const typename Tools::SphericalHarmonics<Samples, Bands>::Sample& a_Sample) const {
        return glm::cross(a_Sample.vec, testValue);
    }
};

template<size_t Samples, size_t Bands>
struct AddVec
{
    constexpr auto operator()(const typename Tools::SphericalHarmonics<Samples, Bands>::Sample& a_Sample) const {
        return a_Sample.vec + testValue;
    }
};

template<size_t Samples, size_t Bands>
struct MultVec
{
    constexpr auto operator()(const typename Tools::SphericalHarmonics<Samples, Bands>::Sample& a_Sample) const {
        return a_Sample.vec * testValue;
    }
};

auto CreateSH() {
    const auto SHTestChrono = Tools::ScopedTimer("SH creation");
    return Tools::SphericalHarmonics<SHSamples, SHBands>();
}

TEST(SH, Constexpr)
{
    //if this test fails it shouldn't compile
    constexpr auto sample = Tools::SphericalHarmonics<SHSamples, SHBands>::Sample(0, 0);
    constexpr auto SHProj = Tools::SphericalHarmonics<5, 4>::StaticEval<AddVec, glm::dvec3>();
}

TEST(SH, AddVec)
{
    const auto SH = CreateSH();
    ASSERT_TRUE(TestFunc<AddVec>(SH, "AddVec"));
}

TEST(SH, MultVec)
{
    const auto SH = CreateSH();
    ASSERT_TRUE(TestFunc<MultVec>(SH, "AddVec"));
}

TEST(SH, CrossVec)
{
    const auto SH = CreateSH();
    ASSERT_TRUE(TestFunc<CrossVec>(SH, "AddVec"));
}

TEST(SH, DotVec)
{
    const auto SH = CreateSH();
    ASSERT_TRUE(TestFunc<DotVec>(SH, "AddVec"));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}