#include <MSG/Cubemap.hpp>
#include <MSG/Entity/Node.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/Renderer/OGL/Components/LightData.hpp>
#include <MSG/Renderer/OGL/RAII/Texture.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Texture.hpp>
#include <MSG/Tools/SphericalHarmonics.hpp>

namespace MSG::Renderer::Component {
template <typename SGLight>
static GLSL::LightCommon ConvertLightCommonData(const uint32_t& a_Type, const SGLight& a_Light, const MSG::Core::Transform& a_Transform)
{
    GLSL::LightCommon common {};
    common.type      = a_Type;
    common.intensity = a_Light.intensity;
    common.color     = a_Light.color;
    common.falloff   = a_Light.falloff;
    common.priority  = a_Light.priority;
    common.radius    = PunctualLight::GetRadius(a_Light);
    common.position  = a_Transform.GetWorldPosition();
    return common;
}

static LightData ConvertLight(Renderer::Impl& a_Renderer, const LightPoint& a_Light, const MSG::Core::Transform& a_Transform)
{
    GLSL::LightPoint glslLight {};
    glslLight.commonData = ConvertLightCommonData(LIGHT_TYPE_POINT, a_Light, a_Transform);
    glslLight.range      = a_Light.range;
    return glslLight;
}

static LightData ConvertLight(Renderer::Impl& a_Renderer, const LightSpot& a_Light, const MSG::Core::Transform& a_Transform)
{
    GLSL::LightSpot glslLight {};
    glslLight.commonData     = ConvertLightCommonData(LIGHT_TYPE_SPOT, a_Light, a_Transform);
    glslLight.range          = a_Light.range;
    glslLight.direction      = a_Transform.GetWorldForward();
    glslLight.innerConeAngle = a_Light.innerConeAngle;
    glslLight.outerConeAngle = a_Light.outerConeAngle;
    return glslLight;
}

static LightData ConvertLight(Renderer::Impl& a_Renderer, const LightDirectional& a_Light, const MSG::Core::Transform& a_Transform)
{
    GLSL::LightDirectional glslLight {};
    glslLight.commonData = ConvertLightCommonData(LIGHT_TYPE_DIRECTIONAL, a_Light, a_Transform);
    glslLight.halfSize   = a_Light.halfSize;
    glslLight.direction  = a_Transform.GetWorldForward();
    return glslLight;
}

static LightData ConvertLight(Renderer::Impl& a_Renderer, const LightIBL& a_Light, const MSG::Core::Transform& a_Transform)
{
    Component::LightIBLData glslLight {};
    glslLight.commonData             = ConvertLightCommonData(LIGHT_TYPE_IBL, a_Light, a_Transform);
    glslLight.halfSize               = a_Light.halfSize;
    glslLight.irradianceCoefficients = a_Light.irradianceCoefficients;
    glslLight.specular               = std::static_pointer_cast<RAII::TextureCubemap>(a_Renderer.LoadTexture(a_Light.specular.texture.get()));
    return glslLight;
}

LightData::LightData(
    Renderer::Impl& a_Renderer,
    const PunctualLight& a_SGLight,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    auto& transform = a_Entity.GetComponent<MSG::Core::Transform>();
    *this           = std::visit([&renderer = a_Renderer, &transform](auto& a_Data) {
        return ConvertLight(renderer, a_Data, transform);
    },
        a_SGLight);
}
}
