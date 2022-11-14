#include <Tools/SphericalHarmonics.hpp>

#include <iostream>
#include <chrono>

using namespace TabGraph;

constexpr auto testValue = glm::dvec3(0, 1, 0);
constexpr auto samplingX = 10;
constexpr auto samplingY = 10;
constexpr auto samplingZ = 10;

struct TestChrono {
    TestChrono(const std::string& a_Name) : name(a_Name) {}
    ~TestChrono() {
        const auto end = std::chrono::steady_clock::now();
        const auto dur = std::chrono::duration<double, std::milli>(end - start);
        std::cout << name << " took " << dur.count() << " ms\n";
    }
    const std::string name;
    const std::chrono::steady_clock::time_point start{ std::chrono::steady_clock::now() };
};

template<template<size_t, size_t> class Op, size_t Samples, size_t Bands>
auto TestFunc(const Tools::SphericalHarmonics<Samples, Bands>& SH, const std::string& a_Name) {
    std::cout << "Test " << a_Name << '\n';
    constexpr Op<Samples, Bands> op{};
    std::array<glm::dvec3, Bands* Bands> SHProj;
    {
        const auto testChrono = TestChrono("SH Evaluation");
        SHProj = SH.ProjectFunction<Op, glm::dvec3>();
    }
    size_t testCount = 0;
    size_t testPassed = 0;
    {
        const auto testChrono = TestChrono("SH Sampling  ");
        for (auto x = -samplingX; x <= samplingX; ++x) {
            for (auto y = -samplingY; y <= samplingY; ++y) {
                for (auto z = -samplingZ; z <= samplingZ; ++z) {
                    ++testCount;
                    Tools::SphericalHarmonics<Samples, Bands>::Sample sample;
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
    std::cout << "Success Rate : " << successRate << "%" << (success ? " [Passed]" : "[Failed]") << '\n';
    return success;
}

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
    const auto SHTestChrono = TestChrono("SH creation");
    return Tools::SphericalHarmonics<100, 4>();
}

int main(int argc, char const *argv[])
{
    //test compilation
    constexpr auto sample = Tools::SphericalHarmonics<100, 4>::Sample(0, 0);
    const auto SH = CreateSH();
    std::cout << "--------------------------------------------------------------------------------\n";
    if (!TestFunc<AddVec>(SH, "AddVec")) return -1;
    std::cout << "--------------------------------------------------------------------------------\n";
    if (!TestFunc<MultVec>(SH, "MultVec")) return -1;
    std::cout << "--------------------------------------------------------------------------------\n";
    if (!TestFunc<CrossVec>(SH, "CrossVec")) return -1;
    std::cout << "--------------------------------------------------------------------------------\n";
    if (!TestFunc<DotVec>(SH, "DotVec")) return -1;
    std::cout << "--------------------------------------------------------------------------------\n";
    return 0;
}