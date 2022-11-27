#pragma once
////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
#include <ECS/EntityRef.hpp>
#include <ECS/EntityStorage.hpp>
#include <ECS/ComponentTypeStorage.hpp>
#include <ECS/View.hpp>
#include <Tools/FixedSizeMemoryPool.hpp>

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
template<typename EntityIDT = uint32_t, size_t MaxEntitiesV = std::numeric_limits<uint16_t>::max(), size_t MaxComponentTypesV = 256>
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

    /** @brief the registry cannot be created on stack because of its size */
    static std::shared_ptr<Registry> Create() {
        return std::shared_ptr<Registry>(new Registry);
    }

    /** @return a reference to a newly created entity */
    EntityRefType CreateEntity();
    /** @return a reference to the specified entity */
    EntityRefType GetEntityRef(EntityIDType a_Entity);
    /** @return true if the specified entity is alive */
    bool IsAlive(EntityIDType a_Entity);

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
    bool HasComponent(EntityIDType a_Entity) const;
    /** @return The component of the specified type */
    template<typename T>
    auto& GetComponent(EntityIDType a_Entity);
    /** @returns A View of the registery with the specified types */
    template<typename... Types>
    auto GetView();

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

    std::array<EntityStorageType*, MaxEntities> _entities;
    Tools::FixedSizeMemoryPool<EntityStorageType, MaxEntities>  _entityPool;
    std::unordered_map<std::type_index, std::shared_ptr<ComponentTypeStorageType>> _componentTypeStorage;
    std::recursive_mutex _lock;
};

/** @copydoc Registry * The default Registry with default template arguments */
typedef Registry<> DefaultRegistry;

template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
inline auto Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::CreateEntity() -> EntityRefType {
    std::scoped_lock lock(_lock);
    auto entityStorage = new(_entityPool.allocate()) EntityStorageType;
    const auto entityID = _entityPool.index_from_addr((std::byte*)entityStorage);
    _entities.at(entityID) = entityStorage;
    return { entityID, this, &entityStorage->refCount };
}
template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
inline auto Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::GetEntityRef(EntityIDType a_Entity) -> EntityRefType {
    std::scoped_lock lock(_lock);
#ifdef _DEBUG
    assert(IsAlive(a_Entity)); //Entity is not alive
#endif
    return { a_Entity, this, &_entities.at(a_Entity)->refCount };
}
template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
inline bool Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::IsAlive(EntityIDType a_Entity) {
    std::scoped_lock lock(_lock);
    return _entities.at(a_Entity) != nullptr;
}
template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template<typename T, typename ...Args>
inline auto& Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::AddComponent(EntityIDType a_Entity, Args && ...a_Args) {
    std::scoped_lock lock(_lock);
    auto& storage = _GetStorage<T>();
#ifdef _DEBUG
    assert(IsAlive(a_Entity)); //Entity is not alive
    assert(!storage.HasComponent(a_Entity)); //Entity already have component type
#endif
    return storage.Allocate(a_Entity, std::forward<Args>(a_Args)...);
}
template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template<typename T>
inline void Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::RemoveComponent(EntityIDType a_Entity) {
    std::scoped_lock lock(_lock);
    auto& storage = _GetStorage<T>();
#ifdef _DEBUG
    assert(IsAlive(a_Entity)); //Entity is not alive
    assert(storage.HasComponent(a_Entity)); //Entity does not have component type
#endif
    storage.Release(a_Entity);
}
template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template<typename T>
inline bool Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::HasComponent(EntityIDType a_Entity) const {
    std::scoped_lock lock(_lock);
    auto& storage = _GetStorage<T>();
#ifdef _DEBUG
    assert(IsAlive(a_Entity)); //Entity is not alive
#endif
    return storage.HasComponent(a_Entity);
}
template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template<typename T>
inline auto& Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::GetComponent(EntityIDType a_Entity) {
    std::scoped_lock lock(_lock);
    auto& storage = _GetStorage<T>();
#ifdef _DEBUG
    assert(IsAlive(a_Entity)); //Entity is not alive
    assert(storage.HasComponent(a_Entity)); //Entity does not have component type
#endif
    return storage.Get(a_Entity);
}
template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template<typename ...Types>
inline auto Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::GetView() {
    std::scoped_lock lock(_lock);
    return View(this, std::ref(_GetStorage<Types>())...);
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
    _entityPool.deallocate((EntityStorageType*)_entityPool.addr_from_index(a_Entity));
}
template<typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template<typename T>
inline auto& Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::_GetStorage() {
    const std::type_index typeID = typeid(T);
    auto componentIt = _componentTypeStorage.find(typeID);
    if (componentIt == _componentTypeStorage.end()) { //component not registered
        auto pool = new ComponentTypeStorage<T, RegistryType>;
        _componentTypeStorage[typeID].reset(pool);
        return *pool;
    }
    return *std::reinterpret_pointer_cast<ComponentTypeStorage<T, RegistryType>>(componentIt->second);
}
}
