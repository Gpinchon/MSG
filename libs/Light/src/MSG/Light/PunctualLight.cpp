#include <MSG/Light/PunctualLight.hpp>

template <typename LightType>
static glm::vec3 MSG::PunctualLight::GetHalfSize(const LightType& a_Light) { return glm::vec3(a_Light.range); }
template <>
static glm::vec3 MSG::PunctualLight::GetHalfSize(const MSG::LightDirectional& a_Light) { return a_Light.halfSize; }
template <>
static glm::vec3 MSG::PunctualLight::GetHalfSize(const MSG::LightIBL& a_Light) { return a_Light.halfSize; }

template <typename LightType>
static float MSG::PunctualLight::GetRadius(const LightType& a_Light) { return a_Light.range; }
template <>
static float MSG::PunctualLight::GetRadius(const MSG::LightDirectional& a_Light) { return glm::length(a_Light.halfSize); }
template <>
static float MSG::PunctualLight::GetRadius(const MSG::LightIBL& a_Light) { return glm::length(a_Light.halfSize); }

glm::vec3 MSG::PunctualLight::GetHalfSize() const
{
    return std::visit([](auto& light) { return GetHalfSize(light); }, *this);
}
float MSG::PunctualLight::GetRadius() const
{
    return std::visit([](auto& light) { return GetRadius(light); }, *this);
}
glm::vec3 MSG::PunctualLight::GetColor() const
{
    return std::visit([](auto& light) { return light.color; }, *this);
}
void MSG::PunctualLight::SetColor(const glm::vec3& a_Value)
{
    std::visit([a_Value](auto& light) { light.color = a_Value; }, *this);
}
float MSG::PunctualLight::GetIntensity() const
{
    return std::visit([](auto& light) { return light.intensity; }, *this);
}
void MSG::PunctualLight::SetIntensity(const float& a_Value)
{
    std::visit([a_Value](auto& light) { light.intensity = a_Value; }, *this);
}
float MSG::PunctualLight::GetFalloff() const
{
    return std::visit([](auto& light) { return light.falloff; }, *this);
}
void MSG::PunctualLight::SetFalloff(const float& a_Value)
{
    std::visit([a_Value](auto& light) { light.falloff = a_Value; }, *this);
}
uint32_t MSG::PunctualLight::GetPriority() const
{
    return std::visit([](auto& light) { return light.priority; }, *this);
}
void MSG::PunctualLight::SetPriority(const uint32_t& a_Value)
{
    std::visit([a_Value](auto& light) { light.priority = a_Value; }, *this);
}
MSG::LightShadowSettings MSG::PunctualLight::GetShadowSettings() const
{
    return std::visit([](auto& light) { return light.shadowSettings; }, *this);
}
void MSG::PunctualLight::SetShadowSettings(const LightShadowSettings& a_Value)
{
    std::visit([a_Value](auto& light) { light.shadowSettings = a_Value; }, *this);
}
bool MSG::PunctualLight::CastsShadow() const
{
    return std::visit([](auto& light) { return light.shadowSettings.castShadow; }, *this);
}