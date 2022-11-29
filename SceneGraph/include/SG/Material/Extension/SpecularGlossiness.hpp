/*
* @Author: gpinchon
* @Date:   2020-10-01 15:30:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-22 20:46:27
*/
#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Property.hpp>
#include <SG/Material/TextureInfo.hpp>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class SpecularGlossinessExtension : public Inherit<Object, SpecularGlossinessExtension> {
    PROPERTY(glm::vec4,     DiffuseFactor, 1, 1, 1, 1);
    PROPERTY(TextureInfo,   DiffuseTexture, );
    PROPERTY(glm::vec3,     SpecularFactor, 1, 1, 1);
    PROPERTY(float,         GlossinessFactor, 1);
    PROPERTY(TextureInfo,   SpecularGlossinessTexture, );
public:
    SpecularGlossinessExtension() : Inherit("SpecularGlossiness") {};
};
}
