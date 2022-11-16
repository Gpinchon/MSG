/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-03 20:05:02
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Property.hpp>
#include <SG/Material/Parameters.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Texture;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class StandardParameters : public Inherit<MaterialParameters, StandardParameters> {
public:
    enum class OpacityMode {
        Opaque,
        Mask,
        Blend,
        MaxValue
    };
    PROPERTY(bool, DoubleSided, false);
    PROPERTY(OpacityMode, OpacityMode, OpacityMode::Opaque);
    PROPERTY(glm::vec2, UVScale, 1);
    PROPERTY(glm::vec2, UVOffset, 0);
    PROPERTY(float, UVRotation, 0);
    PROPERTY(glm::vec3, Diffuse, 0);
    PROPERTY(glm::vec3, Emissive, 0);
    PROPERTY(float, OpacityCutoff, 0);
    PROPERTY(float, Opacity, 0);
    PROPERTY(float, Parallax, 0);
    PROPERTY(float, Ior, 0);
    PROPERTY(std::shared_ptr<Texture>, TextureDiffuse, nullptr);
    PROPERTY(std::shared_ptr<Texture>, TextureEmissive, nullptr);
    PROPERTY(std::shared_ptr<Texture>, TextureNormal, nullptr);
    PROPERTY(std::shared_ptr<Texture>, TextureHeight, nullptr);
    PROPERTY(std::shared_ptr<Texture>, TextureAO, nullptr);
    PROPERTY(std::shared_ptr<Texture>, TextureBRDFLUT, nullptr);

public:
    StandardParameters() : Inherit("Standard") {};
};
}
