#pragma once
////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
#include <ECS/EntityRef.hpp>
#include <ECS/EntityStorage.hpp>
#include <ECS/ComponentTypeStorage.hpp>
#include <ECS/View.hpp>
#include <Tools/FixedSizeMemoryPool.hpp>

#include <gcem.hpp>

#include <cstdint>
#include <limits>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#ifdef _DEBUG
#include <cassert>
#endif

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
/**
* @brief The ECS registry, it keeps track of the entites and components
*/
template<typename EntityIDT = uint32_t, size_t MaxEntitiesV = gcem::pow(2, 17), size_t MaxComponentTypesV = gcem::pow(2, 8)>
class Registry {
public:
    static constexpr auto MaxEntities = MaxEntitiesV;
    static constexpr auto MaxComponentTypes = MaxComponentTypesV;
    typedef EntityIDT                                               EntityIDType;
    typedef Registry<EntityIDType, MaxEntities, MaxComponentTypes>  RegistryType;
    typedef EntityRef<RegistryType>                                 EntityRefType; /** @copydoc EntityRef * The EntityRef type used by this Registry type */
    typedef EntityStorage<RegistryType>                             EntityStorageType;
    typedef ComponentTypeStorageI<EntityIDType>                     ComponentTypeStorageType;
    friend EntityRefType;

    ~Registry() {
#ifdef _DEBUG
        for (const auto& entity : _entities) assert(entity == nullptr && "Some entities outlived the registry");
#endif
        for (auto componentStorage : _componentTypeStorage) {
            delete componentStorage.second;
        }
    }

    /** @brief the registry cannot be created on stack because of its size */
    static std::shared_ptr<Registry> Create() {
        return std::shared_ptr<Registry>(new Registry);
    }

    /** @return a reference to a newly created entity */
    template<typename ...Components>
    [[nodiscard]] EntityRefType CreateEntity();
    /** @return a reference to the specified entity */
    [[nodiscard]] EntityRefType GetEntityRef(EntityIDType a_Entity);
    /** @return true if the specified entity is alive */
    bool IsAlive(EntityIDType a_Entity);
    /** @return the number of alive entities */
    size_t Count();

    /**
    * @brief Constructs a component using the arguments and attaches it to the entity
    * @return the newly created component
    */
    template<typename T, typename... Args>
    auto& AddComponent(EntityIDType a_Entity, Args&&... a_Args);
    /** @brief Removes the component of the specified type from the entity */
    template<typename T>
    void RemoveComponent(EntityIDType a_Entity);
    /** @return true if the entity has a component of the specified type */
    template<typename T>
    bool HasComponent(EntityIDType a_Entity);
    /** @return The component of the specified type */
    template<typename T>
    auto& GetComponent(EntityIDType a_Entity);
    /** @returns A View of the registery with the specified types */
    template<typename... ToGet, typename... ToExclude>
    auto GetView(Exclude<ToExclude...> = {});

    /**
    * @brief It is recomended to lock the Registry before doing multiple operations on it
    * @return this Registry's recursive mutex
    */
    auto& GetLock() { return _lock; }

private:
    Registry();
    template<typename T>
    auto& _GetStorage();
    void _DestroyEntity(EntityIDType a_Entity);

    template<typename Factory>
    struct LazyConstructor {
        using result_type = std::invoke_result_t<const Factory&>;
        constexpr LazyConstructor(Factory&& a_Factory) : factory(a_Factory) {}
        constexpr operator result_type() const noexcept(std::is_nothrow_invocable_v<const Factory&>) {
            return factory();
        }
        const Factory factory;
    };
    std::recursive_mutex _lock;
    std::unordered_map<std::type_index, ComponentTypeStorageType*>  _componentTypeStorage;
    std::array<EntityStorageType*, MaxEntities>                     _entities;
    Tools::FixedSizeMemoryPool<EntityStorageType, MaxEntities>      _entityPool;
};

/** @copydoc Registry * The default Registry with default template arguments */
typedef Registry<> DefaultRegistry;

template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
inline auto Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::GetEntityRef(EntityIDType a_Entity) -> EntityRefType {
    std::scoped_lock lock(_lock);
#ifdef _DEBUG
    assert(IsAlive(a_Entity) && "Entity is not alive");
#endif
    return { a_Entity, this, &_entities.at(a_Entity)->refCount };
}
template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
inline bool Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::IsAlive(EntityIDType a_Entity) {
    std::scoped_lock lock(_lock);
    return _entities.at(a_Entity) != nullptr;
}

template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
inline size_t Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::Count() {
    std::scoped_lock lock(_lock);
    return _entityPool.count();
}

template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template<typename ...Components>
[[nodiscard]]
inline auto Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::CreateEntity() -> EntityRefType {
    std::scoped_lock lock(_lock);
    auto entityStorage = new(_entityPool.allocate()) EntityStorageType;
    const auto entityID = _entityPool.index_from_addr((std::byte*)entityStorage);
    _entities.at(entityID) = entityStorage;
    (..., AddComponent<Components>(entityID));
    return { entityID, this, &entityStorage->refCount };
}

template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template<typename T, typename ...Args>
inline auto& Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::AddComponent(EntityIDType a_Entity, Args && ...a_Args) {
    std::scoped_lock lock(_lock);
    auto& storage = _GetStorage<T>();
#ifdef _DEBUG
    assert(IsAlive(a_Entity) && "Entity is not alive");
    assert(!storage.HasComponent(a_Entity) && "Entity already has this component type");
#endif
    return storage.Allocate(a_Entity, std::forward<Args>(a_Args)...);
}

template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template<typename T>
inline void Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::RemoveComponent(EntityIDType a_Entity) {
    std::scoped_lock lock(_lock);
    auto& storage = _GetStorage<T>();
#ifdef _DEBUG
    assert(IsAlive(a_Entity) && "Entity is not alive");
    assert(storage.HasComponent(a_Entity) && "Entity does not have component type");
#endif
    storage.Release(a_Entity);
}

template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template<typename T>
inline bool Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::HasComponent(EntityIDType a_Entity) {
    std::scoped_lock lock(_lock);
#ifdef _DEBUG
    assert(IsAlive(a_Entity) && "Entity is not alive");
#endif
    auto it = _componentTypeStorage.find(typeid(T));
    return it != _componentTypeStorage.end()
        && reinterpret_cast<ComponentTypeStorage<T, RegistryType>*>(it->second)->HasComponent(a_Entity);
}

template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template<typename T>
inline auto& Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::GetComponent(EntityIDType a_Entity) {
    std::scoped_lock lock(_lock);
    auto& storage = _GetStorage<T>();
#ifdef _DEBUG
    assert(IsAlive(a_Entity) && "Entity is not alive");
    assert(storage.HasComponent(a_Entity) && "Entity does not have component type");
#endif
    return storage.Get(a_Entity);
}

template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template<typename ...ToGet, typename ...ToExclude>
inline auto Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::GetView(Exclude<ToExclude...>) {
    std::scoped_lock lock(_lock);
    return View<RegistryType,
        Get<ComponentTypeStorage<ToGet, RegistryType>&...>,
        Exclude<ComponentTypeStorage<ToExclude, RegistryType>&...>>
        (this, _GetStorage<ToGet>()..., _GetStorage<ToExclude>()...);
}

template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
inline Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::Registry() {
    _componentTypeStorage.reserve(MaxComponentTypes);
    _entities.fill(nullptr);
}
template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
inline void Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::_DestroyEntity(EntityIDType a_Entity) {
    std::scoped_lock lock(_lock);
    for (const auto& pool : _componentTypeStorage) pool.second->Release(a_Entity);
    _entities.at(a_Entity) = nullptr;
    auto ptr = (EntityStorageType*)_entityPool.addr_from_index(a_Entity);
    std::destroy_at(ptr);
    _entityPool.deallocate(ptr);
}

template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template<typename T>
inline auto& Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::_GetStorage() {
    auto [it, second] = _componentTypeStorage.try_emplace(typeid(T), LazyConstructor([]() { return new ComponentTypeStorage<T, RegistryType>; }));
    return *reinterpret_cast<ComponentTypeStorage<T, RegistryType>*>(it->second);
}
}
