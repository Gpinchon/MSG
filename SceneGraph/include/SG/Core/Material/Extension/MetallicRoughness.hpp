/*
* @Author: gpinchon
* @Date:   2020-10-01 15:30:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-18 22:49:43
*/
#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Material/TextureInfo.hpp>
#include <SG/Core/Property.hpp>

#include <glm/vec4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class MetallicRoughnessExtension : public Inherit<Object, MetallicRoughnessExtension> {
    PROPERTY(glm::vec4,     ColorFactor, 1, 1, 1, 1 );
    PROPERTY(TextureInfo,   ColorTexture, );
    PROPERTY(float,         MetallicFactor, 1 );
    PROPERTY(float,         RoughnessFactor, 1 );
    PROPERTY(TextureInfo,   MetallicRoughnessTexture, );
public:
    MetallicRoughnessExtension() : Inherit("MetallicRoughness") {};
};
}
