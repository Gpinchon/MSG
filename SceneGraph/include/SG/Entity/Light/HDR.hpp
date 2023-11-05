/*
 * @Author: gpinchon
 * @Date:   2021-03-14 22:12:40
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-05-04 20:02:25
 */
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Component/Light/Directional.hpp>
#include <SG/Entity/Light/Light.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Light::HDR {
#define DIRECTIONAL_LIGHT_COMPONENTS LIGHT_COMPONENTS, SG::Light::Directional::Settings

uint32_t& GetNbr();
template <typename RegistryType>
auto Create(const RegistryType& a_Registry)
{
    auto entity                 = Light::Directional::Create(a_Registry);
    entity.template GetComponent<Name>() = "HDRLight_" + std::to_string(++GetNbr());
    entity.template AddComponent<Settings>();
    return entity;
}
}
