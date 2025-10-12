#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/FixedSizeMemoryPool.hpp>
#include <MSG/SparseSet.hpp>

#include <array>
#include <cassert>
#include <cstdint>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace Msg::ECS {
template <typename EntityType>
struct ComponentTypeStorageI {
    virtual ~ComponentTypeStorageI() = default;
    virtual void Release(EntityType) = 0;
};

template <typename Type, typename RegistryType>
class ComponentTypeStorage : public ComponentTypeStorageI<typename RegistryType::EntityIDType>, public SparseSet<Type, RegistryType::MaxEntities> {
public:
    using value_type = Type;
    using size_type  = uint32_t;
    typedef typename RegistryType::EntityIDType EntityIDType;

    ComponentTypeStorage()                             = default;
    ComponentTypeStorage(const ComponentTypeStorage&)  = delete;
    ComponentTypeStorage(const ComponentTypeStorage&&) = delete;

    template <typename... Args>
    Type& Allocate(EntityIDType a_Entity, Args&&... a_Args);
    virtual void Release(EntityIDType a_Entity) override;
    bool HasComponent(EntityIDType a_Entity) const;
    auto& Get(EntityIDType a_Entity);
    std::tuple<const value_type&> GetTuple(const EntityIDType a_Entity) const noexcept;
    std::tuple<value_type&> GetTuple(const EntityIDType a_Entity) noexcept;
    auto FirstEntity() const;
    auto LastEntity() const;

private:
    uint32_t _firstEntity { RegistryType::MaxEntities }, _lastEntity { 0 };
};
}

#include <MSG/ECS/ComponentTypeStorage.inl>
