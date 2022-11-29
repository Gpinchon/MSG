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
template<typename RegistryType, typename... Types>
class View {
public:
    /**
    * @brief Executes the specified functor on
    * the entities with the specified components
    */
    template<typename... Args>
    void ForEach(const std::function<void(typename RegistryType::EntityIDType, Args&...)>& a_Func) const;

private:
    friend RegistryType;
    View(RegistryType*, Types&&... a_Types);
    template<typename T>
    auto& _GetStorage() const;
    template <typename... Args>
    auto _Get() const;
    template<typename EntityIDTYpe, typename Storage>
    static inline void _FindEntityRange(EntityIDTYpe& a_FirstEntity, EntityIDTYpe& a_LastEntity, const Storage& a_Storage) {
        a_FirstEntity = std::min(a_Storage.FirstEntity(), a_FirstEntity);
        a_LastEntity = std::max(a_Storage.LastEntity(), a_LastEntity);
    }
    template<typename EntityIDTYpe, typename Storage>
    static inline void _HasComponent(EntityIDTYpe& a_Entity, bool& a_HasComponent, const Storage& a_Storage) {
        if (!a_Storage.HasComponent(a_Entity)) a_HasComponent = false;
    }
    const std::tuple<Types...> _storage;
};

template<typename RegistryType, typename ...Types>
template<typename ...Args>
inline void View<RegistryType, Types...>::ForEach(const std::function<void(typename RegistryType::EntityIDType, Args&...)>& a_Func) const {
    const auto& storages = _Get<Args...>();
    typename RegistryType::EntityIDType firstEntity{ RegistryType::MaxEntities }, lastEntity{ 0 };
    std::apply([&firstEntity, &lastEntity](auto&... ts) {
        (..., _FindEntityRange(firstEntity, lastEntity, ts));
    }, storages);
    while (firstEntity <= lastEntity) {
        bool hasComponents = true;
        std::apply([&firstEntity, &hasComponents](auto&... ts) {
            (..., _HasComponent(firstEntity, hasComponents, ts));
        }, storages);
        if (hasComponents) a_Func(firstEntity, _GetStorage<Args>().get().Get(firstEntity)...);
        ++firstEntity;
    }
}
template<typename RegistryType, typename ...Types>
template<typename T>
inline auto& View<RegistryType, Types...>::_GetStorage() const {
    return std::get<std::reference_wrapper<ComponentTypeStorage<T, RegistryType>>>(_storage);
}
template<typename RegistryType, typename ...Types>
template<typename ...Args>
inline auto View<RegistryType, Types...>::_Get() const {
    return std::make_tuple(std::get<std::reference_wrapper<ComponentTypeStorage<Args, RegistryType>>>(_storage)...);
}
template<typename RegistryType, typename ...Types>
inline View<RegistryType, Types...>::View(RegistryType*, Types && ...a_Types) : _storage(std::make_tuple(a_Types...)) {}
}
