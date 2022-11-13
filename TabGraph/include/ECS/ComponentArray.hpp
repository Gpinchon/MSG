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
    void Insert(Entity entity, const T& component)
    {
        auto index = _size;
        _entityToIndex[entity] = _size;
        _indexToEntity[index] = entity;
        _components.at(index) = component;
        ++_size;
    }
    void Remove(Entity entity)
    {
        auto indexOfRemovedEntity = _entityToIndex.at(entity);
        auto indexOfLastElement = _size - 1;
        _components.at(indexOfRemovedEntity) = _components.at(indexOfLastElement);
        auto entityOfLastElement { _indexToEntity.at(indexOfLastElement) };
        _entityToIndex.at(entityOfLastElement) = indexOfRemovedEntity;
        _indexToEntity.at(indexOfRemovedEntity) = entityOfLastElement;
        _entityToIndex.erase(entity);
        _indexToEntity.erase(indexOfLastElement);
    }
    T& GetComponent(Entity entity)
    {
        return _components.at(entity);
    }
    virtual void EntityDestroyed(Entity entity) override
    {
        if (_entityToIndex.find(entity) != _entityToIndex.end())
            Remove(entity);
    }

private:
    std::array<T, MAX_ENTITIES> _components;
    std::unordered_map<Entity, size_t> _entityToIndex;
    std::unordered_map<size_t, Entity> _indexToEntity;
    size_t _size { 0 };
};
}
