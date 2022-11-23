/*
* @Author: gpinchon
* @Date:   2021-07-27 20:46:17
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-27 20:49:07
*/
#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <ECS/Common.hpp>

#include <array>
#include <queue>
#include <memory>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
class EntityManager {
public:
    EntityManager()
    {
        for (Entity entity = 0u; entity < MAX_ENTITIES; ++entity)
            _freeEntities.push(entity);
    }
    Entity CreateEntity()
    {
        auto id { _freeEntities.front() };
        _freeEntities.pop();
        ++_livingEntities;
        return id;
    }
    Entity GetEntity(Entity a_ID) {
        return a_ID;
    }
    void DestroyEntity(Entity entity) {
        _signatures.at(entity).reset();
        _freeEntities.push(entity);
        --_livingEntities;
    }
    void SetSignature(Entity entity, Signature signature) {
        _signatures.at(entity) = signature;
    }
    Signature GetSignature(Entity entity) {
        return _signatures.at(entity);
    }

private:    
    std::queue<Entity> _freeEntities {};
    std::array<Signature, MAX_ENTITIES> _signatures {};
    size_t _livingEntities { 0 };
};
}