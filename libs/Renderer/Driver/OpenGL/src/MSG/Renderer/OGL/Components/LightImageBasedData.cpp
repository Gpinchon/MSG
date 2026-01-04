#include <MSG/Renderer/OGL/Components/LightImageBasedData.hpp>

#include <MSG/Debug.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/OGLBindlessTextureSampler.hpp>
#include <MSG/OGLTexture.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Texture.hpp>
#include <MSG/Transform.hpp>

namespace Msg::Renderer {
template <typename SGLight>
static GLSL::LightCommon ConvertLightCommonData(const uint32_t& a_Type, const SGLight& a_Light, const Msg::Transform& a_Transform)
{
    GLSL::LightCommon common {};
    common.type                = a_Type;
    common.intensity           = a_Light.intensity;
    common.falloff             = a_Light.falloff;
    common.radius              = PunctualLight::GetRadius(a_Light);
    common.position            = a_Transform.GetWorldPosition();
    common.color               = a_Light.color;
    common.lightShaftIntensity = a_Light.lightShaftIntensity;
    common.priority            = a_Light.priority;
    return common;
}

LightImageBasedData::LightImageBasedData(
    Renderer::Impl& a_Renderer,
    const std::shared_ptr<OGLSampler>& a_Sampler,
    const Msg::PunctualLight& a_Light,
    const Transform& a_Transform)
{
    Update(a_Renderer, a_Sampler, a_Light, a_Transform);
}

void Msg::Renderer::LightImageBasedData::Update(
    Renderer::Impl& a_Renderer,
    const std::shared_ptr<OGLSampler>& a_Sampler,
    const Msg::PunctualLight& a_Light,
    const Transform& a_Transform)
{
    MSGCheckErrorFatal(a_Light.GetType() != LightType::IBL, "Punctual light is not of IBL type !");
    auto& lightIBL  = a_Light.Get<Msg::LightIBL>();
    auto newTexture = a_Renderer.LoadTexture(lightIBL.specular.texture.get());
    if (textureSampler == nullptr || newTexture != textureSampler->texture) {
        textureSampler = std::make_shared<OGLBindlessTextureSampler>(a_Renderer.context, newTexture, a_Sampler);
        specular       = textureSampler->handle;
    }
    commonData       = ConvertLightCommonData(LIGHT_TYPE_IBL, lightIBL, a_Transform);
    boxProjection    = lightIBL.boxProjection;
    innerBoxOffset   = lightIBL.innerBoxOffset;
    innerBoxHalfSize = lightIBL.innerBoxHalfSize;
    halfSize         = lightIBL.halfSize;
    toLightSpace     = glm::inverse(a_Transform.GetWorldTranslationMatrix() * a_Transform.GetWorldRotationMatrix());
    for (uint8_t i = 0; i < 16; i++)
        irradianceCoefficients[i] = glm::vec4(lightIBL.irradianceCoefficients[i], 1);
}
}
