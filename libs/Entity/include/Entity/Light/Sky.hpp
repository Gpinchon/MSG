////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Light/Sky.hpp>
#include <Entity/Light/PunctualLight.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Entity::Light::Sky {
#define SKY_LIGHT_COMPONENTS LIGHT_COMPONENTS, SG::Light::Sky::Settings

uint32_t& GetNbr();
template <typename RegistryType>
auto Create(const RegistryType& a_Registry)
{
    auto entity                          = Light::Directional::Create(a_Registry);
    entity.template GetComponent<Name>() = "SkyLight_" + std::to_string(++GetNbr());
    entity.template AddComponent<Settings>();
    return entity;
}

glm::vec3 GetSkyLight(const glm::vec3& a_Direction, const Settings& sky);
template <typename EntityRefType>
auto GetIncidentLight(const EntityRefType& a_Light, const glm::vec3& direction)
{
    return GetSkyLight(direction, a_Light.template GetComponent<Settings>());
}
}
