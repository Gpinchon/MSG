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
    template<typename T>
    bool SystemRegistered() {
        const std::type_index typeIndex = typeid(T);
        return _systems.find(typeIndex) != _systems.end();
    }
    template <typename T, typename...Args>
    std::shared_ptr<T> RegisterSystem(Args... a_Args)
    {
        std::type_index typeIndex = typeid(T);
        if (SystemRegistered<T>()) return std::static_pointer_cast<T>(_systems.at(typeIndex));
        auto system { std::make_shared<T>(a_Args...) };
        _systems[typeIndex] = system;
        return system;
    }
    template <typename T>
    void SetSignature(Signature signature)
    {
        assert(SystemRegistered<T>() && "System not registered");
        std::type_index typeIndex = typeid(T);
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