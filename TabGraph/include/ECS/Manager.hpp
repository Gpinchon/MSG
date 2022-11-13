/*
* @Author: gpinchon
* @Date:   2021-07-27 20:53:34
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-27 20:56:15
*/
#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <ECS/ComponentManager.hpp>
#include <ECS/EntityManager.hpp>
#include <ECS/SystemManager.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
class Manager {
public:
    ///Entities
    static auto CreateEntity()
    {
        return _Get()._entityManager.CreateEntity();
    }
    static void DestroyEntity(Entity entity)
    {
        _Get()._entityManager.DestroyEntity(entity);
        _Get()._componentManager.EntityDestroyed(entity);
        _Get()._systemManager.EntityDestroyed(entity);
    }
    ///Components
    template <typename T>
    static void RegisterComponent()
    {
        _Get()._componentManager.RegisterComponent<T>();
    }
    template <typename T>
    static void AddComponent(Entity entity, const T& component)
    {
        _Get()._componentManager.AddComponent(entity, component);
        auto signature { _Get()._entityManager.GetSignature(entity) };
        signature.set(_Get()._componentManager.GetComponentType<T>(), true);
        _Get()._entityManager.SetSignature(entity, signature);
        _Get()._systemManager.EntitySignatureChanged(entity, signature);
    }
    template <typename T>
    static T& GetComponent(Entity entity)
    {
        return _Get()._componentManager.GetComponent<T>(entity);
    }
    template <typename T>
    static auto GetComponentType()
    {
        return _Get()._componentManager.GetComponentType<T>();
    }
    ///Systems
    template <typename T>
    static auto RegisterSystem()
    {
        return _Get()._systemManager.RegisterSystem<T>();
    }
    template <typename T>
    static void SetSystemSignature(Signature signature)
    {
        _Get()._systemManager.SetSignature<T>(signature);
    }

private:
    static Manager& _Get();
    EntityManager _entityManager {};
    ComponentManager _componentManager {};
    SystemManager _systemManager {};
};
}