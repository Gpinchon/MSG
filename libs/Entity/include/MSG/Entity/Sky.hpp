#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Entity/PunctualLight.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/Light/Sky.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Entity::LightSky {
#define SKY_LIGHT_COMPONENTS LIGHT_COMPONENTS, MSG::LightSkySettings

uint32_t& GetNbr();
template <typename RegistryType>
auto Create(const RegistryType& a_Registry)
{
    auto entity                                = PunctualLight::Create(a_Registry);
    entity.template GetComponent<Core::Name>() = "SkyLight_" + std::to_string(++GetNbr());
    entity.template AddComponent<LightSkySettings>();
    return entity;
}

template <typename EntityRefType>
auto GetIncidentLight(const EntityRefType& a_Light, const glm::vec3& direction)
{
    return GetSkyLight(direction, a_Light.template GetComponent<LightSkySettings>());
}
}
