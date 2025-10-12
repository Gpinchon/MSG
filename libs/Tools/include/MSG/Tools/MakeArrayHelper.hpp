#pragma once

#include <array>

namespace Msg::Tools {
template <typename T, size_t... Is>
std::array<T, sizeof...(Is)> MakeArrayHelper(
    const T& val, std::index_sequence<Is...>)
{
    return { (static_cast<void>(Is), val)... };
}

template <typename T, size_t N, typename... Args>
std::array<T, N> MakeArray(Args&&... args)
{
    return MakeArrayHelper<T>(T(std::forward<Args>(args)...), std::make_index_sequence<N> {});
}
}