#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Tools/FixedSizeMemoryPool.hpp>

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

template<typename Type, typename RegistryType>//typename EntityType, size_t MaxEntities>
class ComponentTypeStorage : public ComponentTypeStorageI<typename RegistryType::EntityIDType>
{
public:
    using value_type = Type;
    using size_type  = uint32_t;
    typedef typename RegistryType::EntityIDType EntityIDType;

    ComponentTypeStorage();
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
    std::array<Type*, RegistryType::MaxEntities> _components;
    Tools::FixedSizeMemoryPool<Type, RegistryType::MaxEntities>  _memoryPool;
};

template<typename Type, typename RegistryType>
inline ComponentTypeStorage<Type, RegistryType>::ComponentTypeStorage() {
    _components.fill(nullptr);
}
template<typename Type, typename RegistryType>
inline void ComponentTypeStorage<Type, RegistryType>::Release(EntityIDType a_Entity) {
    auto& ptr = _components.at(a_Entity);
    if (ptr != nullptr) {
        std::destroy_at(ptr);
        _memoryPool.deallocate(ptr);
        ptr = nullptr;
        if (_memoryPool.empty()) {
            _firstEntity = RegistryType::MaxEntities;
            _lastEntity = 0;
            return;
        }
        if (_firstEntity == a_Entity && _lastEntity > _firstEntity) {
            do {
                ++_firstEntity;
            } while (_firstEntity < _lastEntity && _components.at(_firstEntity) == nullptr);
        }
        if (_lastEntity == a_Entity && _lastEntity > _firstEntity) {
            do {
                --_lastEntity;
            } while (_firstEntity < _lastEntity && _components.at(_firstEntity) == nullptr);
        }
}
}
template<typename Type, typename RegistryType>
inline bool ComponentTypeStorage<Type, RegistryType>::HasComponent(EntityIDType a_Entity) const {
    return _components.at(a_Entity) != nullptr;
}
template<typename Type, typename RegistryType>
inline auto& ComponentTypeStorage<Type, RegistryType>::Get(EntityIDType a_Entity) {
    return *_components.at(a_Entity);
}
template<typename Type, typename RegistryType>
inline auto ComponentTypeStorage<Type, RegistryType>::GetTuple(const EntityIDType a_Entity) const noexcept -> std::tuple<const value_type&> {
    return std::forward_as_tuple(Get(a_Entity));
}
template<typename Type, typename RegistryType>
inline auto ComponentTypeStorage<Type, RegistryType>::GetTuple(const EntityIDType a_Entity) noexcept -> std::tuple<value_type&> {
    return std::forward_as_tuple(Get(a_Entity));
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
    assert(_memoryPool.free() > 0); //No more free space
#endif
    _firstEntity = std::min(a_Entity, _firstEntity);
    _lastEntity = std::max(a_Entity, _lastEntity);
    return *(_components.at(a_Entity) = new(_memoryPool.allocate()) Type(std::forward<Args>(a_Args)...));
}
}
