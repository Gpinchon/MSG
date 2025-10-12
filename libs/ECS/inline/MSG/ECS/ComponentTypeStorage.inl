#pragma once
namespace Msg::ECS {
template <typename Type, typename RegistryType>
inline void ComponentTypeStorage<Type, RegistryType>::Release(
    EntityIDType a_Entity)
{
    if (!this->contains(a_Entity))
        return;
    this->erase(a_Entity);
    if (this->empty()) [[unlikely]] {
        _firstEntity = RegistryType::MaxEntities;
        _lastEntity  = 0;
        return;
    }
    if (_firstEntity == a_Entity && _lastEntity > _firstEntity) [[unlikely]] {
        do {
            ++_firstEntity;
        } while (_firstEntity < _lastEntity && !this->contains(_firstEntity));
    }

    if (_lastEntity == a_Entity && _lastEntity > _firstEntity) [[unlikely]] {
        do {
            --_lastEntity;
        } while (_firstEntity < _lastEntity && !this->contains(_firstEntity));
    }
}
template <typename Type, typename RegistryType>
inline bool ComponentTypeStorage<Type, RegistryType>::HasComponent(
    EntityIDType a_Entity) const
{
    return this->contains(a_Entity);
}
template <typename Type, typename RegistryType>
inline auto& ComponentTypeStorage<Type, RegistryType>::Get(
    EntityIDType a_Entity)
{
    return this->at(a_Entity);
}
template <typename Type, typename RegistryType>
inline auto ComponentTypeStorage<Type, RegistryType>::GetTuple(
    const EntityIDType a_Entity) const noexcept -> std::tuple<const value_type&>
{
    return std::forward_as_tuple(at(a_Entity));
}
template <typename Type, typename RegistryType>
inline auto ComponentTypeStorage<Type, RegistryType>::GetTuple(
    const EntityIDType a_Entity) noexcept -> std::tuple<value_type&>
{
    return std::forward_as_tuple(at(a_Entity));
}
template <typename Type, typename RegistryType>
inline auto ComponentTypeStorage<Type, RegistryType>::FirstEntity() const
{
    return _firstEntity;
}
template <typename Type, typename RegistryType>
inline auto ComponentTypeStorage<Type, RegistryType>::LastEntity() const
{
    return _lastEntity;
}
template <typename Type, typename RegistryType>
template <typename... Args>
inline Type& ComponentTypeStorage<Type, RegistryType>::Allocate(
    EntityIDType a_Entity, Args&&... a_Args)
{
    assert(!this->full() && "No more free space");
    _firstEntity = std::min(a_Entity, _firstEntity);
    _lastEntity  = std::max(a_Entity, _lastEntity);
    return this->insert(a_Entity, std::forward<Args>(a_Args)...);
}
}
