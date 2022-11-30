/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:56
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-17 23:30:00
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Entity/Light/Light.hpp>
#include <SG/Component/Light/Directional.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Light::Directional {
#define DIRECTIONAL_LIGHT_COMPONENTS LIGHT_COMPONENTS, SG::Light::Directional::Settings

uint32_t& GetNbr();
template<typename RegistryType>
auto Create(const RegistryType& a_Registry) {
    auto entity = Light::Create(a_Registry);
    entity.GetComponent<Name>() = "DirectionalLight_" + std::to_string(++GetNbr());
    entity.AddComponent<Settings>();
    return entity;
}

template<typename EntityRefType>
auto GetMin(const EntityRefType& a_Light) {
    return NodeGetWorldPosition(a_Light) - a_Light.GetComponent<Settings>().halfSize;
}
template<typename EntityRefType>
auto GetMax(const EntityRefType& a_Light) {
    return NodeGetWorldPosition(a_Light) + a_Light.GetComponent<Settings>().halfSize;
}
}
