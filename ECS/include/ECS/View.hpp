#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <functional>
#include <tuple>
#include <utility>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
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
    template <typename Func>
    void ForEach(const Func& a_Func) {
        for (const auto& args : *this) {
            std::apply(
                [&a_Func](IDType a_EntityID, auto&&... a_Args){
                    if constexpr (std::is_invocable<Func, IDType, decltype(a_Args)...>::value)
                        a_Func(a_EntityID, std::forward<decltype(a_Args)>(a_Args)...);
                    else a_Func(std::forward<decltype(a_Args)>(a_Args)...);
                },
                args);
        }
    }

    constexpr Iterator begin();
    constexpr Iterator end();

private:
    friend RegistryType;
    View(RegistryType*, ToGet... a_ToGet, ToExclude... a_ToExclude);
    template <typename EntityIDTYpe, typename Storage>
    static inline void _FindEntityRange(EntityIDTYpe& a_FirstEntity, EntityIDTYpe& a_LastEntity, const Storage& a_Storage);

    const std::tuple<ToGet...> _toGet;
    const std::tuple<ToExclude...> _toExclude;
};

template <typename RegistryType, typename... ToGet, typename... ToExclude>
constexpr auto View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::begin() -> Iterator
{
    IDType currEntity { RegistryType::MaxEntities }, lastEntity { 0 };
    std::apply([&currEntity, &lastEntity](auto&... ts) {
        (..., _FindEntityRange(currEntity, lastEntity, ts));
    },
        _toGet);
    if (lastEntity == 0) //No entities
        lastEntity = RegistryType::MaxEntities;
    else lastEntity++;
    return { _toGet, _toExclude, currEntity, lastEntity };
}

template <typename RegistryType, typename... ToGet, typename... ToExclude>
constexpr auto View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::end() -> Iterator
{
    IDType lastEntity { 0 };
    std::apply([&lastEntity](auto&... ts) {
        (..., (lastEntity = std::max(lastEntity, ts.LastEntity())));
    },
        _toGet);
    if (lastEntity == 0) //No entities
        lastEntity = RegistryType::MaxEntities;
    else lastEntity++;
    return { _toGet, _toExclude, lastEntity, lastEntity };
}

template <typename RegistryType, typename... ToGet, typename... ToExclude>
inline View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::View(RegistryType*, ToGet... a_ToGet, ToExclude... a_ToExclude)
    : _toGet(std::tie(a_ToGet...))
    , _toExclude(std::tie(a_ToExclude...))
{
}

template <typename RegistryType, typename... ToGet, typename... ToExclude>
template <typename EntityIDTYpe, typename Storage>
inline void View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::_FindEntityRange(EntityIDTYpe& a_FirstEntity, EntityIDTYpe& a_LastEntity, const Storage& a_Storage)
{
    a_FirstEntity = std::min(a_Storage.FirstEntity(), a_FirstEntity);
    a_LastEntity  = std::max(a_Storage.LastEntity(), a_LastEntity);
}
template <typename RegistryType, typename... ToGet, typename... ToExclude>
constexpr View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::Iterator::Iterator(const std::tuple<ToGet...>& a_ToGet, const std::tuple<ToExclude...>& a_ToExclude, IDType a_Curr, IDType a_Last)
    : _toGet(a_ToGet)
    , _toExclude(a_ToExclude)
    , _curr(a_Curr)
    , _last(a_Last)
{
    while (_curr < _last && !_IsValid(_curr)) {
        ++_curr;
    }
}

template <typename RegistryType, typename... ToGet, typename... ToExclude>
constexpr auto View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::Iterator::operator++() noexcept -> Iterator&
{
    while (++_curr < _last && !_IsValid(_curr)) {}
    return *this;
}

template <typename RegistryType, typename... ToGet, typename... ToExclude>
[[nodiscard]] constexpr auto View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::Iterator::operator*() const noexcept
{
    return std::make_tuple(_curr, std::ref(std::get<ToGet>(_toGet).Get(_curr))...);
}

template <typename RegistryType, typename... ToGet, typename... ToExclude>
constexpr bool View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::Iterator::_IsValid(IDType a_Entity)
{
    bool contains    = std::apply([a_Entity](const auto&... storage) {
        return (storage.contains(a_Entity) && ...);
    },
        _toGet);
    bool containsNot = std::apply([a_Entity](const auto&... storage) {
        return (!storage.contains(a_Entity) && ...);
    },
        _toExclude);
    return contains && containsNot;
}

}
