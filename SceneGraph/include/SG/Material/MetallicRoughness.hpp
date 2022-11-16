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
#include <SG/Material/Parameters.hpp>
#include <SG/Core/Inherit.hpp>

#include <glm/vec3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Texture;
}

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class MetallicRoughnessParameters : public Inherit<MaterialParameters, MetallicRoughnessParameters> {
    PROPERTY(float, Opacity, 1.0);
    PROPERTY(float, Metallic, 0.0);
    PROPERTY(float, Roughness, 0.5);
    PROPERTY(glm::vec3, Albedo, 1.f);
    PROPERTY(std::shared_ptr<Texture>, TextureAlbedo, nullptr);
    PROPERTY(std::shared_ptr<Texture>, TextureMetallic, nullptr);
    PROPERTY(std::shared_ptr<Texture>, TextureRoughness, nullptr);
    PROPERTY(std::shared_ptr<Texture>, TextureMetallicRoughness, nullptr);

public:
    MetallicRoughnessParameters() : Inherit("MetallicRoughness") {};
};
}

