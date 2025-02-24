#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Light/ShadowCaster.hpp>
#include <MSG/Texture/Sampler.hpp>
#include <MSG/Tools/Pi.hpp>

#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <memory>
#include <optional>
#include <string>
#include <variant>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class Image;
}

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
struct LightBase {
    glm::vec3 color   = { 1.f, 1.f, 1.f };
    float intensity   = 1.f;
    float falloff     = 2.f; // used to compute inverse square root for attenuation
    unsigned priority = 0; // lights with higher priorities will be displayed in priority
    LightShadowSettings shadowSettings;
};

struct LightPoint : LightBase {
    float range = std::numeric_limits<float>::infinity();
};

struct LightSpot : LightBase {
    float range = std::numeric_limits<float>::infinity();
    float innerConeAngle { 0.f };
    float outerConeAngle { M_PIf / 4.f };
};

struct LightDirectional : LightBase {
    glm::vec3 halfSize { std::numeric_limits<float>::infinity() };
};

struct LightIBL : LightBase {
    glm::vec3 halfSize { std::numeric_limits<float>::infinity() };
    LightIBL() = default;
    /// @brief Creates an IBL light from a skybox texture, generating the prefiltered specular map
    LightIBL(const glm::ivec2& a_Size, const std::shared_ptr<Texture>& a_Skybox);
    /// @brief Creates an IBL light from a cubemap image, generating the prefiltered specular map
    LightIBL(const glm::ivec2& a_Size, const std::shared_ptr<Image>& a_Skybox);
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
    auto GetType() const { return LightType(index()); }
    template <typename Type>
    Type& Get() { return std::get<Type>(*this); }
    template <typename Type>
    const Type& Get() const { return std::get<Type>(*this); }
    template <typename Type>
    static float GetRadius(const Type& a_Light);
    template <typename Type>
    static glm::vec3 GetHalfSize(const Type& a_Light);
    glm::vec3 GetHalfSize() const;
    float GetRadius() const;
    glm::vec3 GetColor() const;
    void SetColor(const glm::vec3& a_Value);
    float GetIntensity() const;
    void SetIntensity(const float& a_Value);
    float GetFalloff() const;
    void SetFalloff(const float& a_Value);
    uint32_t GetPriority() const;
    void SetPriority(const uint32_t& a_Value);
    LightShadowSettings GetShadowSettings() const;
    void SetShadowSettings(const LightShadowSettings& a_Value);
    bool CastsShadow() const;
    std::string name;
};
}
