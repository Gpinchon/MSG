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
class SpecularGlossinessParameters : public Inherit<MaterialParameters, SpecularGlossinessParameters> {
    PROPERTY(glm::vec3, Diffuse, 1);
    PROPERTY(glm::vec3, Specular, 1);
    PROPERTY(float, Glossiness, 0.5);
    PROPERTY(float, Opacity, 1);
    PROPERTY(std::shared_ptr<Texture>, TextureDiffuse, nullptr);
    PROPERTY(std::shared_ptr<Texture>, TextureSpecular, nullptr);
    PROPERTY(std::shared_ptr<Texture>, TextureGlossiness, nullptr);
    PROPERTY(std::shared_ptr<Texture>, TextureSpecularGlossiness, nullptr);

public:
    SpecularGlossinessParameters() : Inherit("SpecularGlossiness") {};
};
}