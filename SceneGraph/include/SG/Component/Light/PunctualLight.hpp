#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Tools/Pi.hpp>

#include <glm/vec3.hpp>

#include <array>
#include <memory>
#include <variant>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Cubemap;
}

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
struct LightBase {
    glm::vec3 color = { 1, 1, 1 };
    float intensity = 1;
    float range     = std::numeric_limits<float>::infinity();
    float falloff   = 0;
};

struct LightPoint : LightBase { };

struct LightSpot : LightBase {
    float innerConeAngle { 0 };
    float outerConeAngle { M_PI / 4.0 };
};

struct LightDirectional : LightBase {
    glm::vec3 halfSize { std::numeric_limits<float>::infinity() };
};

struct LightIBL : LightDirectional {
    std::shared_ptr<Cubemap> specular; // a cubemap to be used for reflection
    std::array<glm::vec3, 9> irradianceCoefficients;
};

enum class LightType {
    Unknown = -1,
    Point,
    Spot,
    Directional,
    IBL,
    MaxValue
};

using PunctualLightBase = std::variant<LightPoint, LightSpot, LightDirectional, LightIBL>;

struct PunctualLight : PunctualLightBase {
    using PunctualLightBase::PunctualLightBase;
    auto GetType() const
    {
        return LightType(index());
    }
    Name name;
};
}
