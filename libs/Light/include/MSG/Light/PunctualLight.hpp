#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Component.hpp>
#include <MSG/Core/Name.hpp>
#include <MSG/Light/ShadowCaster.hpp>
#include <MSG/Texture/Sampler.hpp>
#include <MSG/Tools/Pi.hpp>

#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <variant>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
class Image;
}

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
struct LightBase {
    glm::vec3 color           = { 1.f, 1.f, 1.f };
    float intensity           = 1.f;
    float falloff             = 2.f; // used to compute inverse square root for attenuation
    float lightShaftIntensity = 1.f;
    unsigned priority         = 0; // lights with higher priorities will be displayed in priority
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
    LightIBL() = default;
    LightIBL(const LightBase& a_Base)
        : LightBase(a_Base)
    {
    }
    /// @brief Creates an IBL light from a skybox texture, generating the prefiltered specular map
    LightIBL(const glm::ivec2& a_Size, const std::shared_ptr<Texture>& a_Skybox);
    /// @brief Creates an IBL light from a cubemap image, generating the prefiltered specular map
    LightIBL(const glm::ivec2& a_Size, const std::shared_ptr<Image>& a_Skybox);
    void GenerateIrradianceCoeffs();
    /// @brief should the sampling be done using a box projection ?
    glm::vec3 halfSize { std::numeric_limits<float>::infinity() };
    bool boxProjection = true;
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

struct PunctualLight : PunctualLightBase, public Component {
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
    float GetLightShaftIntensity() const;
    void SetLightShaftIntensity(const float& a_Value);
    uint32_t GetPriority() const;
    void SetPriority(const uint32_t& a_Value);
    LightShadowSettings GetShadowSettings() const;
    void SetShadowSettings(const LightShadowSettings& a_Value);
    bool CastsShadow() const;
};
}

template <typename LightType>
inline glm::vec3 Msg::PunctualLight::GetHalfSize(const LightType& a_Light) { return glm::vec3(a_Light.range); }
template <>
inline glm::vec3 Msg::PunctualLight::GetHalfSize(const Msg::LightDirectional& a_Light) { return a_Light.halfSize; }
template <>
inline glm::vec3 Msg::PunctualLight::GetHalfSize(const Msg::LightIBL& a_Light) { return a_Light.halfSize; }

template <typename LightType>
inline float Msg::PunctualLight::GetRadius(const LightType& a_Light) { return a_Light.range; }
template <>
inline float Msg::PunctualLight::GetRadius(const Msg::LightDirectional& a_Light) { return glm::length(a_Light.halfSize); }
template <>
inline float Msg::PunctualLight::GetRadius(const Msg::LightIBL& a_Light) { return glm::length(a_Light.halfSize); }