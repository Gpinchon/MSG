/*
* @Author: gpinchon
* @Date:   2019-06-28 13:27:57
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:44:56
*/

#pragma once

#include <Tools/SequenceTable.hpp>

template<uint64_t N, typename T = uint64_t>
struct FactorialTraits : std::integral_constant<T, N * FactorialTraits<N - 1>::value> {};

template<>
struct FactorialTraits<0> : std::integral_constant<uint8_t, 1> {};

static_assert(FactorialTraits<12, uint32_t>::value == 479001600, "");
static_assert(FactorialTraits<19, uint64_t>::value == 121645100408832000, "");

template<uint64_t N, typename T = uint64_t>
struct DoubleFactorialTraits : std::integral_constant<T, N * DoubleFactorialTraits<N - 2>::value> {};

template<>
struct DoubleFactorialTraits<1> : std::integral_constant<uint8_t, 1> {};

template<>
struct DoubleFactorialTraits<0> : std::integral_constant<uint8_t, 1> {};

static_assert(DoubleFactorialTraits<12, uint32_t>::value == 46080, "");
static_assert(DoubleFactorialTraits<19, uint64_t>::value == 654729075, "");
static_assert(DoubleFactorialTraits<33, uint64_t>::value == 6332659870762850625, "");

constexpr inline auto Factorial(uint8_t x)
{
    constexpr auto table{ SequenceTable<uint64_t, 19, FactorialTraits>() };
    return table.at(x); //you cannot compute above 19!
}

constexpr inline auto DoubleFactorial(uint8_t x)
{
    constexpr auto table{ SequenceTable<uint64_t, 34, DoubleFactorialTraits>() };
    return table.at(x); //you cannot compute above 33!!
}
