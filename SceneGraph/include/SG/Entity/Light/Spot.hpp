#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Entity/Light/Light.hpp>

#include <SG/Component/Light/Spot.hpp>

#include <Tools/Pi.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Light::Spot {
#define SPOT_LIGHT_COMPONENTS LIGHT_COMPONENTS, SG::Light::Spot::Settings

uint32_t& GetNbr();
template<typename RegistryType>
auto Create(const RegistryType& a_Registry) {
    auto entity = Light::Create(a_Registry);
    entity.GetComponent<Name>() = "SpotLight_" + std::to_string(++GetNbr());
    entity.AddComponent<Settings>();
    return entity;
}
}
