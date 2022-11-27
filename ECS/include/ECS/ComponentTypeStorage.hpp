#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Tools/FixedSizeMemoryPool.hpp>

#include <stdint.h>
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

template<typename Type, typename EntityType, size_t MaxEntities>
class ComponentTypeStorage : public ComponentTypeStorageI<EntityType>
{
public:
    using value_type = Type;
    using size_type  = uint32_t;

    ComponentTypeStorage() {
        _components.fill(nullptr);
    }
    template<typename... Args>
    Type& Allocate(EntityType a_Entity, Args&&... a_Args) {
        _firstEntity = std::min(a_Entity, _firstEntity);
        _lastEntity = std::max(a_Entity, _lastEntity);
        return *(_components.at(a_Entity) = new(_memoryPool.allocate()) Type(std::forward<Args>(a_Args)...));
    }
    virtual void Release(EntityType a_Entity) override {
        auto& ptr = _components.at(a_Entity);
        if (ptr != nullptr) {
            std::destroy_at(ptr);
            _memoryPool.deallocate(ptr);
            ptr = nullptr;
            if (_memoryPool.empty()) {
                _firstEntity = MaxEntities;
                _lastEntity  = 0;
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
    bool HasComponent(EntityType a_Entity) const {
        return _components.at(a_Entity) != nullptr;
    }
    auto& Get(EntityType a_Entity) {
        return *_components.at(a_Entity);
    }
    std::tuple<const value_type&> GetTuple(const EntityType a_Entity) const noexcept {
        return std::forward_as_tuple(Get(a_Entity));
    }
    std::tuple<value_type&> GetTuple(const EntityType a_Entity) noexcept {
        return std::forward_as_tuple(Get(a_Entity));
    }
    auto FirstEntity() const {
        return _firstEntity;
    }
    auto LastEntity() const {
        return _lastEntity;
    }

private:
    uint32_t _firstEntity{ MaxEntities }, _lastEntity{ 0 };
    std::array<Type*, MaxEntities> _components;
    Tools::FixedSizeMemoryPool<Type, MaxEntities>  _memoryPool;
};
}