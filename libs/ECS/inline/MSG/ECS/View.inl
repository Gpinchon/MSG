#pragma once

namespace MSG::ECS {
template <typename RegistryType, typename... ToGet, typename... ToExclude>
constexpr auto View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::begin() -> Iterator
{
    IDType currEntity { RegistryType::MaxEntities }, lastEntity { 0 };
    std::apply([&currEntity, &lastEntity](auto&... ts) {
        (..., _FindEntityRange(currEntity, lastEntity, ts));
    },
        _toGet);
    if (lastEntity == 0) // No entities
        lastEntity = RegistryType::MaxEntities;
    else
        lastEntity++;
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
    if (lastEntity == 0) // No entities
        lastEntity = RegistryType::MaxEntities;
    else
        lastEntity++;
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
constexpr auto View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::Iterator::operator++() noexcept -> iterator_type&
{
    while (++_curr < _last && !_IsValid(_curr)) { }
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
