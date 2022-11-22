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

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
class IComponentArray {
public:
    virtual void EntityDestroyed(EntityID entity) = 0;
};

template <typename T>
class ComponentArray : public IComponentArray {
public:
    void Insert(EntityID entity, const T& component)
    {
        assert(!HasComponent(entity) && "Component added to same entity more than once.");
        auto index = _size;
        _entityToIndex[entity] = _size;
        _indexToEntity[index] = entity;
        _components.at(index) = component;
        ++_size;
    }
    void Remove(EntityID entity)
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
    bool HasComponent(EntityID entity) {
        return _entityToIndex.find(entity) != _entityToIndex.end();
    }
    T& GetComponent(EntityID entity)
    {
        return _components.at(_entityToIndex.at(entity));
    }
    virtual void EntityDestroyed(EntityID entity) override
    {
        if (HasComponent(entity)) Remove(entity);
    }

private:
    std::array<T, MAX_ENTITIES> _components;
    std::unordered_map<EntityID, size_t> _entityToIndex;
    std::unordered_map<size_t, EntityID> _indexToEntity;
    size_t _size { 0 };
};
}
