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

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
class EntityManager {
public:
    EntityManager()
    {
        for (EntityID entity = 0u; entity < MAX_ENTITIES; ++entity)
            _freeEntities.push(entity);
    }
    EntityID CreateEntity()
    {
        auto id { _freeEntities.front() };
        _freeEntities.pop();
        ++_livingEntities;
        return id;
    }
    void DestroyEntity(EntityID entity)
    {
        _signatures.at(entity).reset();
        _freeEntities.push(entity);
        --_livingEntities;
    }
    void SetSignature(EntityID entity, Signature signature)
    {
        _signatures.at(entity) = signature;
    }
    Signature GetSignature(EntityID entity)
    {
        return _signatures.at(entity);
    }

private:
    std::queue<EntityID> _freeEntities {};
    std::array<Signature, MAX_ENTITIES> _signatures {};
    size_t _livingEntities { 0 };
};
}