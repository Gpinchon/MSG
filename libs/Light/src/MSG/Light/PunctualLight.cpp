#include <MSG/Light/PunctualLight.hpp>

glm::vec3 Msg::PunctualLight::GetHalfSize() const
{
    return std::visit([](auto& light) { return GetHalfSize(light); }, *this);
}
float Msg::PunctualLight::GetRadius() const
{
    return std::visit([](auto& light) { return GetRadius(light); }, *this);
}
glm::vec3 Msg::PunctualLight::GetColor() const
{
    return std::visit([](auto& light) { return light.color; }, *this);
}
void Msg::PunctualLight::SetColor(const glm::vec3& a_Value)
{
    std::visit([a_Value](auto& light) { light.color = a_Value; }, *this);
}
float Msg::PunctualLight::GetIntensity() const
{
    return std::visit([](auto& light) { return light.intensity; }, *this);
}
void Msg::PunctualLight::SetIntensity(const float& a_Value)
{
    std::visit([a_Value](auto& light) { light.intensity = a_Value; }, *this);
}
float Msg::PunctualLight::GetFalloff() const
{
    return std::visit([](auto& light) { return light.falloff; }, *this);
}
void Msg::PunctualLight::SetFalloff(const float& a_Value)
{
    std::visit([a_Value](auto& light) { light.falloff = a_Value; }, *this);
}
float Msg::PunctualLight::GetLightShaftIntensity() const
{
    return std::visit([](auto& light) { return light.lightShaftIntensity; }, *this);
}
void Msg::PunctualLight::SetLightShaftIntensity(const float& a_Value)
{
    std::visit([a_Value](auto& light) { light.lightShaftIntensity = a_Value; }, *this);
}
uint32_t Msg::PunctualLight::GetPriority() const
{
    return std::visit([](auto& light) { return light.priority; }, *this);
}
void Msg::PunctualLight::SetPriority(const uint32_t& a_Value)
{
    std::visit([a_Value](auto& light) { light.priority = a_Value; }, *this);
}
Msg::LightShadowSettings Msg::PunctualLight::GetShadowSettings() const
{
    return std::visit([](auto& light) { return light.shadowSettings; }, *this);
}
void Msg::PunctualLight::SetShadowSettings(const LightShadowSettings& a_Value)
{
    std::visit([a_Value](auto& light) { light.shadowSettings = a_Value; }, *this);
}
bool Msg::PunctualLight::CastsShadow() const
{
    return std::visit([](auto& light) { return light.shadowSettings.castShadow; }, *this);
}