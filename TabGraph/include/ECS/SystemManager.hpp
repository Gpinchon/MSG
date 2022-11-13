/*
* @Author: gpinchon
* @Date:   2021-07-27 20:52:36
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-27 20:53:18
*/

#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <ECS/System.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
class SystemManager {
public:
    template <typename T>
    std::shared_ptr<T> RegisterSystem()
    {
        std::type_index typeIndex = typeid(T);
        auto system { std::make_shared<T>() };
        _systems[typeIndex] = system;
        return system;
    }
    template <typename T>
    void SetSignature(Signature signature)
    {
        std::type_index typeIndex = typeid(T);
        assert(_systems.find(typeIndex) != _systems.end() && "System not registered");
        _signatures[typeIndex] = signature;
    }
    void EntityDestroyed(Entity entity)
    {
        for (const auto& pair : _systems)
            pair.second->entities.erase(entity);
    }
    void EntitySignatureChanged(Entity entity, ECS::Signature signature)
    {
        for (const auto& pair : _systems) {
            const auto& type { pair.first };
            const auto& system { pair.second };
            const auto& systemSignature { _signatures.at(type) };
            if ((signature & systemSignature) == systemSignature)
                system->entities.insert(entity);
            else
                system->entities.erase(entity);
        }
    }

private:
    std::unordered_map<std::type_index, Signature> _signatures;
    std::unordered_map<std::type_index, std::shared_ptr<System>> _systems;
};
}