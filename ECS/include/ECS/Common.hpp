/*
* @Author: gpinchon
* @Date:   2021-07-27 20:44:56
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-27 20:48:50
*/
#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <bitset>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
using EntityID = uint16_t;
constexpr auto MAX_ENTITIES = std::numeric_limits<EntityID>::max();

using ComponentType = uint8_t;
constexpr auto MAX_COMPONENTS = std::numeric_limits<ComponentType>::max();

using Signature = std::bitset<MAX_COMPONENTS>;
}