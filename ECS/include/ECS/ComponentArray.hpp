/*
* @Author: gpinchon
* @Date:   2021-07-27 20:47:31
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-27 20:49:25
*/
#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <ECS/Common.hpp>

#include <unordered_map>
#include <cassert>
#include <memory_resource>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
class IComponentArray {
public:
    virtual void EntityDestroyed(Entity entity) = 0;
};

template <typename T>
class ComponentArray : public IComponentArray {
public:
    template<typename ...Args>
    void Insert(Entity entity, Args... a_Args)
    {
        assert(!HasComponent(entity) && "Component added to same entity more than once.");
        auto index = _size;
        _entityToIndex[entity] = _size;
        _indexToEntity[index] = entity;
        _components.at(index) = std::allocate_shared<T>(_allocator, a_Args...);
        ++_size;
    }
    void Remove(Entity entity)
    {
        assert(HasComponent(entity) && "Removing non-existent component.");
        auto indexOfRemovedEntity = _entityToIndex.at(entity);
        auto indexOfLastElement = _size - 1;
        _components.at(indexOfRemovedEntity) = _components.at(indexOfLastElement);
        auto entityOfLastElement { _indexToEntity.at(indexOfLastElement) };
        _entityToIndex[entityOfLastElement] = indexOfRemovedEntity;
        _indexToEntity[indexOfRemovedEntity] = entityOfLastElement;
        _entityToIndex.erase(entity);
        _indexToEntity.erase(indexOfLastElement);
        --_size;
    }
    bool HasComponent(Entity entity) {
        return _entityToIndex.find(entity) != _entityToIndex.end();
    }
    std::shared_ptr<T> GetComponent(Entity a_Entity)
    {
        if (HasComponent(a_Entity))
            return _components.at(_entityToIndex.at(a_Entity));
        return nullptr;
    }
    virtual void EntityDestroyed(Entity entity) override
    {
        if (HasComponent(entity)) Remove(entity);
    }

private:
    size_t                                       _size{ 0 };
    std::array<std::shared_ptr<T>, MAX_ENTITIES> _components;
    std::byte                                    _memory[sizeof(T) * MAX_ENTITIES];
    std::pmr::monotonic_buffer_resource          _mbr{ _memory, sizeof(_memory) };
    std::pmr::polymorphic_allocator<T>           _allocator{ &_mbr };

    std::unordered_map<Entity, size_t>           _entityToIndex;
    std::unordered_map<size_t, Entity>           _indexToEntity;
};
}
