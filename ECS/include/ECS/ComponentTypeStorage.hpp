#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Tools/FixedSizeMemoryPool.hpp>
#include <Tools/SparseSet.hpp>

#include <cstdint>
#include <array>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
template<typename EntityType>
struct ComponentTypeStorageI
{
    virtual void Release(EntityType) = 0;
};

template<typename Type, typename RegistryType>
class ComponentTypeStorage : public ComponentTypeStorageI<typename RegistryType::EntityIDType>, Tools::SparseSet<Type, RegistryType::MaxEntities>
{
public:
    using value_type = Type;
    using size_type  = uint32_t;
    typedef typename RegistryType::EntityIDType EntityIDType;

    ComponentTypeStorage() = default;
    ComponentTypeStorage(const ComponentTypeStorage&) = delete;
    ComponentTypeStorage(const ComponentTypeStorage&&) = delete;

    template<typename... Args>
    Type& Allocate(EntityIDType a_Entity, Args&&... a_Args);
    virtual void Release(EntityIDType a_Entity) override;
    bool HasComponent(EntityIDType a_Entity) const;
    auto& Get(EntityIDType a_Entity);
    std::tuple<const value_type&> GetTuple(const EntityIDType a_Entity) const noexcept;
    std::tuple<value_type&> GetTuple(const EntityIDType a_Entity) noexcept;
    auto FirstEntity() const;
    auto LastEntity() const;

private:
    uint32_t _firstEntity{ RegistryType::MaxEntities }, _lastEntity{ 0 };
};

template<typename Type, typename RegistryType>
inline void ComponentTypeStorage<Type, RegistryType>::Release(EntityIDType a_Entity) {
    if (!contains(a_Entity)) return;
    erase(a_Entity);
    if (empty()) {
        _firstEntity = RegistryType::MaxEntities;
        _lastEntity = 0;
        return;
    }
    if (_firstEntity == a_Entity && _lastEntity > _firstEntity) {
        do {
            ++_firstEntity;
        } while (_firstEntity < _lastEntity && !contains(_firstEntity));
    }

    if (_lastEntity == a_Entity && _lastEntity > _firstEntity) {
        do {
            --_lastEntity;
        } while (_firstEntity < _lastEntity && !contains(_firstEntity));
    }
}
template<typename Type, typename RegistryType>
inline bool ComponentTypeStorage<Type, RegistryType>::HasComponent(EntityIDType a_Entity) const {
    return contains(a_Entity);
}
template<typename Type, typename RegistryType>
inline auto& ComponentTypeStorage<Type, RegistryType>::Get(EntityIDType a_Entity) {
    return at(a_Entity);
}
template<typename Type, typename RegistryType>
inline auto ComponentTypeStorage<Type, RegistryType>::GetTuple(const EntityIDType a_Entity) const noexcept -> std::tuple<const value_type&> {
    return std::forward_as_tuple(at(a_Entity));
}
template<typename Type, typename RegistryType>
inline auto ComponentTypeStorage<Type, RegistryType>::GetTuple(const EntityIDType a_Entity) noexcept -> std::tuple<value_type&> {
    return std::forward_as_tuple(at(a_Entity));
}
template<typename Type, typename RegistryType>
inline auto ComponentTypeStorage<Type, RegistryType>::FirstEntity() const {
    return _firstEntity;
}
template<typename Type, typename RegistryType>
inline auto ComponentTypeStorage<Type, RegistryType>::LastEntity() const {
    return _lastEntity;
}
template<typename Type, typename RegistryType>
template<typename ...Args>
inline Type& ComponentTypeStorage<Type, RegistryType>::Allocate(EntityIDType a_Entity, Args && ...a_Args) {
#ifdef _DEBUG
    assert(!full() && "No more free space");
#endif
    _firstEntity = std::min(a_Entity, _firstEntity);
    _lastEntity = std::max(a_Entity, _lastEntity);
    return insert(a_Entity, std::forward<Args>(a_Args)...);
}
}
