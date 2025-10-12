#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/ECS/ApplyMax.hpp>
#include <functional>
#include <tuple>
#include <utility>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace Msg::ECS {
template <typename... T>
struct Get { };
template <typename... T>
struct Exclude { };

// To make compiler happy
template <typename, typename, typename>
class View;

template <typename RegistryType, typename... ToGet, typename... ToExclude>
class View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>> {
public:
    typedef typename RegistryType::EntityIDType IDType;
    class Iterator {
    public:
        using iterator_type     = View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::Iterator;
        using iterator_category = std::forward_iterator_tag;
        constexpr Iterator& operator++() noexcept;
        [[nodiscard]] constexpr auto operator*() const noexcept;

        friend constexpr bool operator==(const Iterator& a_Left, const Iterator& a_Right)
        {
            return a_Left._curr == a_Right._curr;
        }
        friend constexpr bool operator!=(const Iterator& a_Left, const Iterator& a_Right)
        {
            return !(a_Left == a_Right);
        }

    private:
        friend View;
        constexpr Iterator(const std::tuple<ToGet...>& a_ToGet, const std::tuple<ToExclude...>& a_ToExclude, IDType a_Curr, IDType a_Last);
        constexpr bool _IsValid(IDType a_Entity);
        const std::tuple<ToGet...>& _toGet;
        const std::tuple<ToExclude...>& _toExclude;
        IDType _curr, _last;
    };

    /**
     * @brief Executes the specified functor on
     * the entities with the specified components.
     */
    template <typename Func, typename... Args>
    void ForEach(const Func& a_Func)
    {
        for (const auto& args : *this) {
            std::apply(
                [&a_Func](auto&&... a_Args) {
                    ApplyMax(a_Func, std::forward<decltype(a_Args)>(a_Args)...);
                },
                args);
        }
    }

    constexpr Iterator begin();
    constexpr Iterator end();
    constexpr bool empty() { return begin() == end(); }

private:
    friend RegistryType;
    View(RegistryType*, ToGet... a_ToGet, ToExclude... a_ToExclude);
    template <typename EntityIDTYpe, typename Storage>
    static inline void _FindEntityRange(EntityIDTYpe& a_FirstEntity, EntityIDTYpe& a_LastEntity, const Storage& a_Storage);

    const std::tuple<ToGet...> _toGet;
    const std::tuple<ToExclude...> _toExclude;
};
}

#include <MSG/ECS/View.inl>
