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
template<typename... T> struct Get{};
template<typename... T> struct Exclude{};

//To make compiler happy
template<typename, typename, typename>
class View;

template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
class View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>> {
public:
    typedef typename RegistryType::EntityIDType IDType;
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        constexpr Iterator& operator++() noexcept;
        [[nodiscard]] constexpr auto operator*() const noexcept;

        friend constexpr bool operator==(const Iterator& a_Left, const Iterator& a_Right) {
            return a_Left._curr == a_Right._curr;
        }
        friend constexpr bool operator!=(const Iterator& a_Left, const Iterator& a_Right) {
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
    * This variant gives the entity ID alongside the components
    */
    template<typename ...Args>
    void ForEach(const std::function<void(IDType, Args&...)>& a_Func) const;

    /**
    * @brief Executes the specified functor on
    * the entities with the specified components.
    * This variant only gives the components
    */
    template<typename ...Args>
    void ForEach(const std::function<void(Args&...)>& a_Func) const;

    constexpr Iterator begin();
    constexpr Iterator end();

private:
    friend RegistryType;
    View(RegistryType*, ToGet ...a_ToGet, ToExclude ...a_ToExclude);
    template<typename EntityIDTYpe, typename Storage>
    static inline void _FindEntityRange(EntityIDTYpe& a_FirstEntity, EntityIDTYpe& a_LastEntity, const Storage& a_Storage);

    const std::tuple<ToGet...> _toGet;
    const std::tuple<ToExclude...> _toExclude;
};

template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
constexpr auto View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::begin() -> Iterator {
    IDType currEntity{ RegistryType::MaxEntities }, lastEntity{ 0 };
    std::apply([&currEntity, &lastEntity](auto&... ts) {
        (..., _FindEntityRange(currEntity, lastEntity, ts));
        }, _toGet);
    return { _toGet, _toExclude, currEntity, lastEntity + 1 };
}

template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
constexpr auto View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::end() -> Iterator {
    IDType lastEntity{ 0 };
    std::apply([&lastEntity](auto&... ts) {
        (..., (lastEntity = std::max(lastEntity, ts.LastEntity())));
        }, _toGet);
    return { _toGet, _toExclude, lastEntity + 1, lastEntity + 1 };
}

template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
inline View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::View(RegistryType*, ToGet ...a_ToGet, ToExclude ...a_ToExclude)
    : _toGet(std::tie(a_ToGet...))
    , _toExclude(std::tie(a_ToExclude...))
{}

template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
template<typename ...Args>
inline void View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::ForEach(const std::function<void(IDType, Args&...)>& a_Func) const {
    const auto& toGet = std::tie(std::get<ComponentTypeStorage<Args, RegistryType>&>(_toGet)...);
    IDType currEntity{ RegistryType::MaxEntities }, lastEntity{ 0 };
    std::apply([&currEntity, &lastEntity](auto&... ts) {
        (..., _FindEntityRange(currEntity, lastEntity, ts));
    }, toGet);
    while (currEntity <= lastEntity) {
        bool get = std::apply([&currEntity](auto&... ts) {
            return (ts.contains(currEntity) &&...);
        }, _toGet);
        bool exclude = std::apply([&currEntity](auto&... ts) {
            return (ts.contains(currEntity) ||...);
        }, _toExclude);;
        if (get && !exclude) a_Func(currEntity, std::get<ComponentTypeStorage<Args, RegistryType>&>(toGet).Get(currEntity)...);
        ++currEntity;
    }
}
template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
template<typename ...Args>
inline void View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::ForEach(const std::function<void(Args&...)>& a_Func) const {
    auto func = [&a_Func](IDType, auto&... a_Args) { a_Func(a_Args...); };
    ForEach<Args...>(func);
}

template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
template<typename EntityIDTYpe, typename Storage>
inline void View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::_FindEntityRange(EntityIDTYpe& a_FirstEntity, EntityIDTYpe& a_LastEntity, const Storage& a_Storage) {
    a_FirstEntity = std::min(a_Storage.FirstEntity(), a_FirstEntity);
    a_LastEntity = std::max(a_Storage.LastEntity(), a_LastEntity);
}
template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
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

template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
constexpr auto View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::Iterator::operator++() noexcept -> Iterator& {
    while (++_curr < _last && !_IsValid(_curr)) {}
    return *this;
}

template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
[[nodiscard]] constexpr auto View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::Iterator::operator*() const noexcept {
    return std::make_tuple(_curr, std::ref(std::get<ToGet>(_toGet).Get(_curr))...);
}

template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
constexpr bool View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::Iterator::_IsValid(IDType a_Entity) {
    return
        std::apply([a_Entity](const auto& ...storage) {
        return (storage.contains(a_Entity) && ...);
        }, _toGet) &&
        std::apply([a_Entity](const auto& ...storage) {
                return (!storage.contains(a_Entity) && ...);
        }, _toExclude);
}

}
