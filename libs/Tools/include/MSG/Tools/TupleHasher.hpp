#pragma once

#include <MSG/Tools/HashCombine.hpp>

#include <tuple>

namespace MSG::Tools {
template <typename... Types>
struct TupleHasher {
    using type = std::tuple<Types...>;
    constexpr inline auto operator()(type const& a_Tuple) const noexcept
    {
        std::size_t seed = 0;
        std::apply(
            [&seed](auto&&... a_Args) {
                ((MSG_HASH_COMBINE(seed, a_Args)), ...);
            },
            a_Tuple);
        return seed;
    }
};
}
