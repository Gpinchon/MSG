#include <Tools/SphericalHarmonics.hpp>

#include <iostream>

using namespace TabGraph;

template<typename T>
constexpr glm::dvec3 SampleSH(const glm::dvec3& N, const T& SH)
{
    const glm::dvec3 N2 = N * N;
    glm::dvec3 v{ 0 };
    
    v += SH[0] * Tools::SHCoeff(0,  0, N);

    v += SH[1] * Tools::SHCoeff(1, -1, N);
    v += SH[2] * Tools::SHCoeff(1,  0, N);
    v += SH[3] * Tools::SHCoeff(1,  1, N);

    v += SH[4] * Tools::SHCoeff(2, -2, N);
    v += SH[5] * Tools::SHCoeff(2, -1, N);
    v += SH[6] * Tools::SHCoeff(2,  0, N);
    v += SH[7] * Tools::SHCoeff(2,  1, N);
    v += SH[8] * Tools::SHCoeff(2,  2, N);

    v += SH[9]  * Tools::SHCoeff(3, -3, N);
    v += SH[10] * Tools::SHCoeff(3, -2, N);
    v += SH[11] * Tools::SHCoeff(3, -1, N);
    v += SH[12] * Tools::SHCoeff(3,  0, N);
    v += SH[13] * Tools::SHCoeff(3,  1, N);
    v += SH[14] * Tools::SHCoeff(3,  2, N);
    v += SH[15] * Tools::SHCoeff(3,  3, N);

    return v;
}

constexpr auto testValue = glm::dvec3(0, 1, 0);

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

template<typename T>
constexpr T Round(T x, unsigned p)
{
    const auto precision = gcem::pow(T(10), p);
    return gcem::round(x * precision) / precision;
}

template<size_t Samples, size_t Bands, template<size_t, size_t> class Op, typename T>
bool TestSH(const T& SH, const glm::dvec3& a_N)
{
    constexpr Op<Samples, Bands> op{};
    Tools::SphericalHarmonics<Samples, Bands>::Sample sample;
    sample.vec = a_N;
    const auto expected = op(sample);
    const auto result = SampleSH(a_N, SH);
    if (!Tools::feq(expected.x, result.x, 0.05)) return false;
    if (!Tools::feq(expected.y, result.y, 0.05)) return false;
    if (!Tools::feq(expected.z, result.z, 0.05)) return false;
    return true;
}

template<template<size_t, size_t> class Op, size_t Samples, size_t Bands>
auto TestFunc(const Tools::SphericalHarmonics<Samples, Bands>& SH) {
    const auto SHProj = SH.ProjectFunction<Op, glm::dvec3>();
    size_t testCount = 0;
    size_t testPassed = 0;
    for (auto x = -10; x <= 10; ++x) {
        for (auto y = -10; y <= 10; ++y) {
            for (auto z = -10; z <= 10; ++z) {
                auto result = TestSH<Samples, Bands, Op>(SHProj, glm::normalize(glm::dvec3(x, y, z)));
                if (result) ++testPassed;
                ++testCount;
            }
        }
    }
    const auto successRate = (testPassed / double(testCount) * 100.0);
    
    return successRate;
}

#include <chrono>

int main(int argc, char const *argv[])
{
    //test compilation
    constexpr auto sample = Tools::SphericalHarmonics<100, 4>::Sample(0, 0);
    const auto SHCreationbegin = std::chrono::steady_clock::now();
    const Tools::SphericalHarmonics<100, 4> SH{};
    const auto SHCreationEnd = std::chrono::steady_clock::now();
    const auto SHCreationDur = std::chrono::duration<double, std::milli>(SHCreationEnd - SHCreationbegin);
    std::cout << "SH creation took " << SHCreationDur.count() << " ms\n";
    std::cout << "--------------------------------------------------------------------------------\n";
    {
        const auto testBegin = std::chrono::steady_clock::now();
        const auto testResult = TestFunc<AddVec>(SH);
        const auto testEnd = std::chrono::steady_clock::now();
        const auto testDur = std::chrono::duration<double, std::milli>(testEnd - testBegin);
        std::cout << "Test AddVec\n";
        std::cout << "Success Rate : " << testResult << "%" << (testResult >= 80 ? " [Passed]" : "[Failed]") << '\n';
        std::cout << "Test took " << testDur.count() << " ms\n";
    }
    std::cout << "--------------------------------------------------------------------------------\n";
    {
        const auto testBegin = std::chrono::steady_clock::now();
        const auto testResult = TestFunc<CrossVec>(SH);
        const auto testEnd = std::chrono::steady_clock::now();
        const auto testDur = std::chrono::duration<double, std::milli>(testEnd - testBegin);
        std::cout << "Test CrossVec\n";
        std::cout << "Success Rate : " << testResult << "%" << (testResult >= 80 ? " [Passed]" : "[Failed]") << '\n';
        std::cout << "Test took " << testDur.count() << " ms\n";
    }
    std::cout << "--------------------------------------------------------------------------------\n";
    {
        const auto testBegin = std::chrono::steady_clock::now();
        const auto testResult = TestFunc<DotVec>(SH);
        const auto testEnd = std::chrono::steady_clock::now();
        const auto testDur = std::chrono::duration<double, std::milli>(testEnd - testBegin);
        std::cout << "Test DotVec\n";
        std::cout << "Success Rate : " << testResult << "%" << (testResult >= 80 ? " [Passed]" : "[Failed]") << '\n';
        std::cout << "Test took " << testDur.count() << " ms\n";
    }
    std::cout << "--------------------------------------------------------------------------------\n";
    return 0;
}