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
#include <mutex>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS::Manager {
class Instance {
public:
    ///Entities
    auto CreateEntity() {
        auto lock = std::scoped_lock<std::mutex>(_lock);
        return _entityManager.CreateEntity();
    }
    auto DestroyEntity(Entity entity) {
        auto lock = std::scoped_lock<std::mutex>(_lock);
        _entityManager.DestroyEntity(entity);
        _componentManager.EntityDestroyed(entity);
        _systemManager.EntityDestroyed(entity);
    }
    ///Components
    template <typename T>
    auto RegisterComponent() {
        auto lock = std::scoped_lock<std::mutex>(_lock);
        _componentManager.RegisterComponent<T>();
    }
    template<typename T, typename...Args>
    auto AddComponent(Entity a_Entity, Args... a_Args) {
        auto lock = std::scoped_lock<std::mutex>(_lock);
        _componentManager.AddComponent<T>(a_Entity, a_Args...);
        auto signature { _entityManager.GetSignature(a_Entity) };
        signature.set(_componentManager.GetComponentType<T>(), true);
        _entityManager.SetSignature(a_Entity, signature);
        _systemManager.EntitySignatureChanged(a_Entity, signature);
    }
    template <typename T>
    void RemoveComponent(Entity a_Entity) {
        auto lock = std::scoped_lock<std::mutex>(_lock);
        _componentManager.RemoveComponent<T>(a_Entity);
        auto signature{ _entityManager.GetSignature(a_Entity) };
        signature.set(_componentManager.GetComponentType<T>(), false);
        _entityManager.SetSignature(a_Entity, signature);
        _systemManager.EntitySignatureChanged(a_Entity, signature);
    }
    template <typename T>
    auto GetComponent(Entity entity) {
        auto lock = std::scoped_lock<std::mutex>(_lock);
        return _componentManager.GetComponent<T>(entity);
    }
    template <typename T>
    auto GetComponentType() {
        auto lock = std::scoped_lock<std::mutex>(_lock);
        return _componentManager.GetComponentType<T>();
    }
    ///Systems
    template <typename T>
    auto RegisterSystem() {
        auto lock = std::scoped_lock<std::mutex>(_lock);
        return _systemManager.RegisterSystem<T>();
    }
    template <typename T>
    void SetSystemSignature(Signature signature) {
        auto lock = std::scoped_lock<std::mutex>(_lock);
        _systemManager.SetSignature<T>(signature);
    }
    auto& GetLock() { return _lock; }
    static Instance& GetGlobal();

private:
    EntityManager       _entityManager;
    ComponentManager    _componentManager;
    SystemManager       _systemManager;
    std::mutex          _lock;
};

inline auto CreateEntity() { return Instance::GetGlobal().CreateEntity(); };
inline auto DestroyEntity(Entity a_Entity) { return Instance::GetGlobal().DestroyEntity(a_Entity); }
template <typename T>
inline auto RegisterComponent() { return Instance::GetGlobal().RegisterComponent<T>(); }
template<typename T, typename...Args>
inline auto AddComponent(Entity a_Entity, Args... a_Args) { return Instance::GetGlobal().AddComponent<T>(a_Entity, a_Args...); }
template <typename T>
inline auto RemoveCOmponent(Entity a_Entity) { return Instance::GetGlobal().RemoveComponent<T>(a_Entity); }
template <typename T>
inline auto GetComponent(Entity a_Entity) { return Instance::GetGlobal().GetComponent<T>(a_Entity); }
template <typename T>
inline auto GetComponentType() { return Instance::GetGlobal().GetComponentType<T>(); }
template <typename T>
inline auto RegisterSystem() { return Instance::GetGlobal().RegisterSystem<T>(); }
template <typename T>
inline auto SetSystemSignature(Signature a_Signature) { return Instance::GetGlobal().SetSystemSignature<T>(a_Signature); }

}