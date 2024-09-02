#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Texture/TextureSampler.hpp>
#include <Tools/Pi.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <memory>
#include <string>
#include <variant>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Cubemap;
struct TextureSampler;
}

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
struct LightBase {
    glm::vec3 color   = { 1.f, 1.f, 1.f };
    float intensity   = 1.f;
    float falloff     = 0.f;
    unsigned priority = 0; // lights with higher priorities will be displayed in priority
};

struct LightPoint : LightBase {
    float range = std::numeric_limits<float>::infinity();
};

struct LightSpot : LightPoint {
    float innerConeAngle { 0.f };
    float outerConeAngle { M_PIf / 4.f };
};

struct LightDirectional : LightBase {
    glm::vec3 halfSize { std::numeric_limits<float>::infinity() };
};

struct LightIBL : LightDirectional {
    LightIBL() = default;
    /// @brief Creates an IBL light from a skybox texture, generating the prefiltered specular map
    LightIBL(const glm::ivec2& a_Size, const std::shared_ptr<Texture>& a_Skybox);
    /// @brief Creates an IBL light from a skybox, generating the prefiltered specular map
    LightIBL(const glm::ivec2& a_Size, const std::shared_ptr<Cubemap>& a_Skybox);
    /// @brief the prefiltered specular map
    TextureSampler specular;
    std::array<glm::vec3, 16> irradianceCoefficients;
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
    std::string name;
};
}
