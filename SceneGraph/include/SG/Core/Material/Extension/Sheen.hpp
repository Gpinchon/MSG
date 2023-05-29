#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Material/TextureInfo.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>

#include <glm/vec3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class SheenExtension : public Inherit<Object, SheenExtension> {
    PROPERTY(glm::vec3, ColorFactor, 0, 0, 0);
    PROPERTY(TextureInfo, ColorTexture, );
    PROPERTY(float, RoughnessFactor, 0);
    PROPERTY(TextureInfo, RoughnessTexture, );

public:
    SheenExtension()
        : Inherit("Sheen") {};
};
}
