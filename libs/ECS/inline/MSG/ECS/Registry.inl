#pragma once
namespace MSG::ECS {
template <typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
inline auto Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::GetEntityRef(const EntityIDType& a_Entity) -> EntityRefType
{
    std::scoped_lock lock(_lock);
    assert(IsAlive(a_Entity) && "Entity is not alive");
    return { a_Entity, this, &_entities.at(a_Entity)->refCount };
}
template <typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
inline bool Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::IsAlive(const EntityIDType& a_Entity) const
{
    std::scoped_lock lock(_lock);
    return _entities.at(a_Entity) != nullptr;
}

template <typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
inline size_t Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::Count() const
{
    std::scoped_lock lock(_lock);
    return _entityPool.count();
}

template <typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
inline size_t Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::GetEntityRefCount(const EntityIDType& a_Entity)
{
    std::scoped_lock lock(_lock);
    return _entities.at(a_Entity)->refCount;
}

template <typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template <typename... Components>
[[nodiscard]] inline auto Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::CreateEntity() -> EntityRefType
{
    std::scoped_lock lock(_lock);
    auto entityStorage     = new (_entityPool.allocate()) EntityStorageType;
    const auto entityID    = _entityPool.index_from_addr((std::byte*)entityStorage);
    _entities.at(entityID) = entityStorage;
    (..., AddComponent<Components>(entityID));
    return { entityID, this, &entityStorage->refCount };
}

template <typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template <typename T, typename... Args>
inline auto& Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::AddComponent(const EntityIDType& a_Entity, Args&&... a_Args)
{
    std::scoped_lock lock(_lock);
    auto& storage = _GetStorage<T>();
    assert(IsAlive(a_Entity) && "Entity is not alive");
    assert(!storage.HasComponent(a_Entity) && "Entity already has this component type");
    return storage.Allocate(a_Entity, std::forward<Args>(a_Args)...);
}

template <typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template <typename T>
inline void Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::RemoveComponent(const EntityIDType& a_Entity)
{
    std::scoped_lock lock(_lock);
    auto& storage = _GetStorage<T>();
    assert(IsAlive(a_Entity) && "Entity is not alive");
    assert(storage.HasComponent(a_Entity) && "Entity does not have component type");
    storage.Release(a_Entity);
}

template <typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template <typename T>
inline bool Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::HasComponent(const EntityIDType& a_Entity) const
{
    std::scoped_lock lock(_lock);
    assert(IsAlive(a_Entity) && "Entity is not alive");
    auto it = _componentTypeStorage.find(typeid(T));
    return it != _componentTypeStorage.end()
        && reinterpret_cast<ComponentTypeStorage<T, RegistryType>*>(it->second)->HasComponent(a_Entity);
}

template <typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template <typename T>
inline T& Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::GetComponent(const EntityIDType& a_Entity)
{
    std::scoped_lock lock(_lock);
    assert(IsAlive(a_Entity) && "Entity is not alive");
    auto it = _componentTypeStorage.find(typeid(T));
    // we should crash if component is not registered or if entity does not have this component attached
    return reinterpret_cast<ComponentTypeStorage<T, RegistryType>*>(it->second)->Get(a_Entity);
}

template <typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template <typename T>
inline const T& Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::GetComponent(const EntityIDType& a_Entity) const
{
    std::scoped_lock lock(_lock);
    assert(IsAlive(a_Entity) && "Entity is not alive");
    auto it = _componentTypeStorage.find(typeid(T));
    // we should crash if component is not registered or if entity does not have this component attached
    return reinterpret_cast<ComponentTypeStorage<T, RegistryType>*>(it->second)->Get(a_Entity);
}

template <typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template <typename... ToGet, typename... ToExclude>
inline auto Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::GetView(Exclude<ToExclude...>)
{
    std::scoped_lock lock(_lock);
    return View<RegistryType,
        Get<ComponentTypeStorage<ToGet, RegistryType>&...>,
        Exclude<ComponentTypeStorage<ToExclude, RegistryType>&...>>(this, _GetStorage<ToGet>()..., _GetStorage<ToExclude>()...);
}

template <typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
inline Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::Registry()
{
    _componentTypeStorage.reserve(MaxComponentTypes);
    _entities.fill(nullptr);
}
template <typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
inline void Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::_DestroyEntity(const EntityIDType& a_Entity)
{
    std::scoped_lock lock(_lock);
    for (const auto& pool : _componentTypeStorage)
        pool.second->Release(a_Entity);
    _entities.at(a_Entity) = nullptr;
    auto ptr               = (EntityStorageType*)_entityPool.addr_from_index(a_Entity);
    std::destroy_at(ptr);
    _entityPool.deallocate(ptr);
}

template <typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template <typename T>
inline auto& Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::_GetStorage()
{
    auto [it, second] = _componentTypeStorage.try_emplace(typeid(T), LazyConstructor([]() { return new ComponentTypeStorage<T, RegistryType>; }));
    return *reinterpret_cast<ComponentTypeStorage<T, RegistryType>*>(it->second);
}

template <typename EntityIDT, size_t MaxEntitiesV, size_t MaxComponentTypesV>
template <typename T>
inline auto& Registry<EntityIDT, MaxEntitiesV, MaxComponentTypesV>::_GetStorage() const
{
    auto it = _componentTypeStorage.find(typeid(T));
    if (it != _componentTypeStorage.end())
        return *reinterpret_cast<ComponentTypeStorage<T, RegistryType>*>(it->second);
    else
        return nullptr;
}
}
