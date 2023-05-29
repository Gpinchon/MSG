#pragma once

#include <type_traits>

namespace TabGraph::Tools {
/**
 * @brief A lazy contructor to use with try_emplace in order to avoid allocating pointers when not needed
 */
template <typename Factory>
struct LazyConstructor {
    using result_type = std::invoke_result_t<const Factory&>;
    constexpr LazyConstructor(Factory&& a_Factory)
        : factory(a_Factory)
    {
    }
    constexpr operator result_type() const noexcept(std::is_nothrow_invocable_v<const Factory&>)
    {
        return factory();
    }
    const Factory factory;
};
}
