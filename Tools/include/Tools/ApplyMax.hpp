#pragma once

#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>

namespace TabGraph::Tools {
template <size_t i, typename F, typename... Ts, size_t... idxs>
constexpr bool CanCallFunctor(std::index_sequence<idxs...>)
{
    return std::is_invocable_v<F, std::tuple_element_t<idxs, std::tuple<Ts...>>...>;
}

template <size_t i, typename F, typename... Ts>
constexpr bool CanCall = CanCallFunctor<i, F, Ts...>(std::make_index_sequence<i> {});

static_assert(CanCall<1, int(int), int>);

template <size_t i, typename F, typename... Ts>
struct DoCall {
    auto operator()(const F& a_Func, Ts&&... a_Args)
    {
        return [&a_Func, &a_Args...]<size_t... idxs>(std::index_sequence<idxs...>) {
            return std::invoke(a_Func, std::get<idxs>(std::tuple<Ts...> { a_Args... })...);
        }(std::make_index_sequence<i> {});
    }
};

template <size_t i, typename F, typename... Ts>
struct X : std::conditional_t<CanCall<i, F, Ts...>, DoCall<i, F, Ts...>, X<i - 1, F, Ts...>> { };

template <typename F, typename... Ts>
struct X<0, F, Ts...> {
    auto operator()(const F& a_Func, Ts&&...)
    {
        return a_Func();
    }
};

/**
 * @brief Expands the variadic arguments to call the function with
 * as much arguments as possible
 */
template <typename F, typename... Ts>
auto ApplyMax(const F& a_Func, Ts&&... a_Args)
{
    return X<sizeof...(Ts), F, Ts...> {}(a_Func, a_Args...);
}
}
