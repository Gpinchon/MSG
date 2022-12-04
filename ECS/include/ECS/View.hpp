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
    /**
    * @brief Executes the specified functor on
    * the entities with the specified components.
    * This variant gives the entity ID alongside the components
    */
    template<typename ...Args>
    void ForEach(const std::function<void(typename RegistryType::EntityIDType, Args&...)>& a_Func) const;

    /**
    * @brief Executes the specified functor on
    * the entities with the specified components.
    * This variant only gives the components
    */
    template<typename ...Args>
    void ForEach(const std::function<void(Args&...)>& a_Func) const;

private:
    friend RegistryType;
    View(RegistryType*, ToGet ...a_ToGet, ToExclude ...a_ToExclude);
    template<typename EntityIDTYpe, typename Storage>
    static inline void _FindEntityRange(EntityIDTYpe& a_FirstEntity, EntityIDTYpe& a_LastEntity, const Storage& a_Storage);
    template<typename EntityIDTYpe, typename Storage>
    static inline void _ToGet(EntityIDTYpe& a_Entity, bool& a_HasComponent, const Storage& a_Storage);
    template<typename EntityIDTYpe, typename Storage>
    static inline void _ToExclude(EntityIDTYpe& a_Entity, bool& a_HasComponent, const Storage& a_Storage);

    const std::tuple<ToGet...> _toGet;
    const std::tuple<ToExclude...> _toExclude;
};

template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
inline View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::View(RegistryType*, ToGet ...a_ToGet, ToExclude ...a_ToExclude)
    : _toGet(std::tie(a_ToGet...))
    , _toExclude(std::tie(a_ToExclude...))
{}

template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
template<typename ...Args>
inline void View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::ForEach(const std::function<void(typename RegistryType::EntityIDType, Args&...)>& a_Func) const {
    const auto& toGet = std::tie(std::get<ComponentTypeStorage<Args, RegistryType>&>(_toGet)...);
    typename RegistryType::EntityIDType firstEntity{ RegistryType::MaxEntities }, lastEntity{ 0 };
    std::apply([&firstEntity, &lastEntity](auto&... ts) {
        (..., _FindEntityRange(firstEntity, lastEntity, ts));
    }, toGet);
    while (firstEntity <= lastEntity) {
        bool get = true;
        bool exclude = false;
        std::apply([&firstEntity, &get](auto&... ts) {
            (..., _ToGet(firstEntity, get, ts));
        }, toGet);
        std::apply([&firstEntity, &exclude](auto&... ts) {
            (..., _ToExclude(firstEntity, exclude, ts));
        }, _toExclude);
        if (get && !exclude) a_Func(firstEntity, std::get<ComponentTypeStorage<Args, RegistryType>&>(toGet).Get(firstEntity)...);
        ++firstEntity;
    }
}
template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
template<typename ...Args>
inline void View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::ForEach(const std::function<void(Args&...)>& a_Func) const {
    auto func = [&a_Func](typename RegistryType::EntityIDType, auto&... a_Args) { a_Func(a_Args...); };
    ForEach<Args...>(func);
}

template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
template<typename EntityIDTYpe, typename Storage>
inline void View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::_FindEntityRange(EntityIDTYpe& a_FirstEntity, EntityIDTYpe& a_LastEntity, const Storage& a_Storage) {
    a_FirstEntity = std::min(a_Storage.FirstEntity(), a_FirstEntity);
    a_LastEntity = std::max(a_Storage.LastEntity(), a_LastEntity);
}
template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
template<typename EntityIDTYpe, typename Storage>
inline void View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::_ToGet(EntityIDTYpe& a_Entity, bool& a_HasComponent, const Storage& a_Storage) {
    if (!a_Storage.HasComponent(a_Entity)) a_HasComponent = false;
}
template<typename RegistryType, typename ...ToGet, typename ...ToExclude>
template<typename EntityIDTYpe, typename Storage>
inline void View<RegistryType, Get<ToGet...>, Exclude<ToExclude...>>::_ToExclude(EntityIDTYpe& a_Entity, bool& a_HasComponent, const Storage& a_Storage) {
    if (a_Storage.HasComponent(a_Entity)) a_HasComponent = true;
}
}
