#pragma once

#include <gcem.hpp>

#include <array>
#include <cstdint>

namespace TabGraph::Tools {
/**
* @arg B : Base of the Halton Sequence
*/
template<uint16_t B>
struct Halton {
    static constexpr auto Value(size_t i) noexcept {
        auto x = 1.0 / double(B);
        auto v = 0.0;
        while (i > 0) {
            v += x * (i % B);
            i /= B;
            x /= double(B);
        }
        return v;
    }
    /**
     * @brief : returns a lookup table of Halton Sequence
     * @arg N : Size of the lookup table
     */
    template<size_t N>
    static constexpr auto Sequence() noexcept {
        std::array<double, N> res{};
        for (size_t i = 1; i < N + 1; ++i)
            res[i - 1] = Value(i);
        return res;
    }
};

/**
 * @brief : returns the value of the Halton Sequence at the specified Index
 * @arg B : Base of the Halton Sequence
 * @arg N : Size of the lookup table
 * @arg a_Index : the index to compute
 */
template<uint16_t B, size_t N>
inline constexpr auto HaltonSequence(const size_t a_Index) noexcept
{
    constexpr const auto table = Halton<B>::Sequence<N>();
    if (a_Index < N)
        return table[a_Index];
    return Halton<B>::Value(a_Index);
}
}
