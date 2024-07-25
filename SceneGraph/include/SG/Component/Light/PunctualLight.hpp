#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Tools/Pi.hpp>

#include <glm/vec3.hpp>

#include <memory>
#include <variant>
#include <vector>

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
    uint priority   = 0; // lights with higher priorities will be displayed in priority
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
    std::vector<std::shared_ptr<Cubemap>> specular; // an array of cubemaps, each index representing a mipmap
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
