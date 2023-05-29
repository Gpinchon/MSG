/*
 * @Author: gpinchon
 * @Date:   2021-03-12 16:01:21
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-05-04 20:02:25
 */

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Component/Light/Sky.hpp>
#include <SG/Entity/Light/Directional.hpp>
////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Light::Sky {
#define SKY_LIGHT_COMPONENTS DIRECTIONAL_LIGHT_COMPONENTS, SG::Light::Sky::Settings

uint32_t& GetNbr();
template <typename RegistryType>
auto Create(const RegistryType& a_Registry)
{
    auto entity                 = Light::Directional::Create(a_Registry);
    entity.GetComponent<Name>() = "SkyLight_" + std::to_string(++GetNbr());
    entity.AddComponent<Settings>();
    return entity;
}

glm::vec3 GetSkyLight(const glm::vec3& a_Direction, const Settings& sky);
template <typename EntityRefType>
auto GetIncidentLight(const EntityRefType& a_Light, const glm::vec3& direction)
{
    return GetSkyLight(direction, a_Light.GetComponent<Settings>());
}
}
