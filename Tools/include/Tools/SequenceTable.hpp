/*
 * @Author: gpinchon
 * @Date:   2019-06-28 13:27:57
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-01-11 08:44:56
 */

#pragma once

#include <array>
#include <utility>

template <typename T, template <uint64_t, typename> class SequenceTraits, uint64_t... Is>
inline constexpr auto GenerateSequenceTable(std::integer_sequence<uint64_t, Is...>) noexcept
{
    return std::array<T, sizeof...(Is)> { { SequenceTraits<Is, T>::value... } };
}

/**
 * @brief generates a number sequence using the traits
 * @arg T : the type of the array
 * @arg N : the size of the sequence
 * @arg SequenceTraits : the traits used to generate the sequence, muse be of type :
 * template<uint64_t N, typename T = uint64_t>
 *     struct Traits : std::integral_constant
 */
template <typename T, uint64_t N, template <uint64_t, typename> class SequenceTraits>
inline constexpr auto SequenceTable() noexcept
{
    return GenerateSequenceTable<T, SequenceTraits>(std::make_integer_sequence<uint64_t, N>());
}
