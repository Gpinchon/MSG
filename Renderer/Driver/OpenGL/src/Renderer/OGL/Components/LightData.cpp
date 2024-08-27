#include <Renderer/OGL/Components/LightData.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/Renderer.hpp>
#include <SG/Component/Light/PunctualLight.hpp>
#include <SG/Core/Image/Cubemap.hpp>
#include <SG/Core/Texture/Texture.hpp>
#include <SG/Entity/Node.hpp>
#include <Tools/SphericalHarmonics.hpp>

namespace TabGraph::Renderer::Component {
LightData::LightData(
    Renderer::Impl& a_Renderer,
    const SG::Component::PunctualLight& a_SGLight,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    GLSL::LightBase glslLight;
    std::visit([&glslLight, &a_Entity](auto& a_Data) {
        glslLight.commonData.position  = SG::Node::GetWorldPosition(a_Entity);
        glslLight.commonData.intensity = a_Data.intensity;
        glslLight.commonData.color     = a_Data.color;
        glslLight.commonData.falloff   = a_Data.falloff;
        glslLight.commonData.priority  = a_Data.priority;
    },
        a_SGLight);
    switch (a_SGLight.GetType()) {
    case SG::Component::LightType::Point: {
        glslLight.commonData.type = LIGHT_TYPE_POINT;
        auto& point               = reinterpret_cast<GLSL::LightPoint&>(glslLight);
        point.range               = std::get<SG::Component::LightPoint>(a_SGLight).range;
        *this                     = point;
    } break;
    case SG::Component::LightType::Spot: {
        glslLight.commonData.type = LIGHT_TYPE_SPOT;
        auto& spot                = reinterpret_cast<GLSL::LightSpot&>(glslLight);
        spot.direction            = SG::Node::GetForward(a_Entity);
        spot.range                = std::get<SG::Component::LightSpot>(a_SGLight).range;
        spot.innerConeAngle       = std::get<SG::Component::LightSpot>(a_SGLight).innerConeAngle;
        spot.outerConeAngle       = std::get<SG::Component::LightSpot>(a_SGLight).outerConeAngle;
        *this                     = spot;
    } break;
    case SG::Component::LightType::Directional: {
        glslLight.commonData.type = LIGHT_TYPE_DIRECTIONAL;
        auto& dirLight            = reinterpret_cast<GLSL::LightDirectional&>(glslLight);
        dirLight.halfSize         = std::get<SG::Component::LightDirectional>(a_SGLight).halfSize;
        *this                     = dirLight;
    } break;
    case SG::Component::LightType::IBL: {
        glslLight.commonData.type = LIGHT_TYPE_IBL;
        Component::LightIBLData IBL;
        auto& lightIBL             = std::get<SG::Component::LightIBL>(a_SGLight);
        IBL.commonData             = glslLight.commonData;
        IBL.halfSize               = lightIBL.halfSize;
        IBL.irradianceCoefficients = lightIBL.irradianceCoefficients;
        IBL.specular               = std::static_pointer_cast<RAII::TextureCubemap>(a_Renderer.LoadTexture(lightIBL.specular.texture.get()));
        *this                      = IBL;
    } break;
    default:
        break;
    }
}
}