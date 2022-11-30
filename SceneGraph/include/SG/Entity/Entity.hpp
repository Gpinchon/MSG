#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Component/Name.hpp>

#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Entity {
#define ENTITY_COMPONENTS SG::Component::Name
/** @return the total nbr of Entities created since start-up */
uint32_t& GetNbr();

template<typename RegistryType>
inline auto Create(const RegistryType& a_Registry) {
    auto entity = a_Registry->CreateEntity();
    entity.AddComponent<SG::Component::Name>("Entity_" + std::to_string(++GetNbr()));
    return entity;
}
}