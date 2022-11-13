/*
* @Author: gpinchon
* @Date:   2021-07-27 20:50:16
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-27 20:51:10
*/
#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <ECS/Common.hpp>
#include <ECS/ComponentArray.hpp>

#include <cassert>
#include <memory>
#include <typeindex>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
class ComponentManager {
public:
    template <typename T>
    void RegisterComponent()
    {
        std::type_index typeIndex = typeid(T);
        assert(_componentTypes.find(typeIndex) == _componentTypes.end() && "Type already registered");
        _componentTypes[typeIndex] = _nextComponentType;
        _componentArrays[typeIndex] = std::make_shared<ComponentArray<T>>();
        ++_nextComponentType;
    }
    template <typename T>
    auto GetComponentType() const
    {
        std::type_index typeIndex = typeid(T);
        return _componentTypes.at(typeIndex);
    }
    template <typename T>
    void AddComponent(Entity entity, const T& component)
    {
        return GetComponentArray<T>()->Insert(entity, component);
    }
    template <typename T>
    void RemoveComponent(Entity entity, const T& component)
    {
        return GetComponentArray<T>()->Remove(entity);
    }
    template <typename T>
    T& GetComponent(Entity entity)
    {
        return GetComponentArray<T>()->GetComponent(entity);
    }
    void EntityDestroyed(Entity entity)
    {
        for (const auto& pair : _componentArrays) {
            auto& component { pair.second };
            component->EntityDestroyed(entity);
        }
    }

private:
    std::unordered_map<std::type_index, ComponentType> _componentTypes;
    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> _componentArrays;
    ComponentType _nextComponentType { 0 };
    template <typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray()
    {
        std::type_index typeIndex = typeid(T);
        //if this crashes, it means the component type was not registered
        return std::static_pointer_cast<ComponentArray<T>>(_componentArrays.at(typeIndex));
    }
};
}
