#include <MSG/Camera/Projection.hpp>
#include <MSG/Entity/Node.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLTextureCubemap.hpp>
#include <MSG/Renderer/OGL/Components/LightData.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Texture.hpp>
#include <MSG/Tools/SphericalHarmonics.hpp>

#include <GL/glew.h>

namespace MSG::Renderer::Component {
template <typename SGLight>
static GLSL::LightCommon ConvertLightCommonData(const uint32_t& a_Type, const SGLight& a_Light, const MSG::Transform& a_Transform)
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

static LightData ConvertLight(Renderer::Impl& a_Renderer, const LightPoint& a_Light, const MSG::Transform& a_Transform)
{
    GLSL::LightPoint glslLight {};
    glslLight.commonData = ConvertLightCommonData(LIGHT_TYPE_POINT, a_Light, a_Transform);
    glslLight.range      = a_Light.range;
    return glslLight;
}

static LightData ConvertLight(Renderer::Impl& a_Renderer, const LightSpot& a_Light, const MSG::Transform& a_Transform)
{
    GLSL::LightSpot glslLight {};
    glslLight.commonData     = ConvertLightCommonData(LIGHT_TYPE_SPOT, a_Light, a_Transform);
    glslLight.range          = a_Light.range;
    glslLight.direction      = a_Transform.GetWorldForward();
    glslLight.innerConeAngle = a_Light.innerConeAngle;
    glslLight.outerConeAngle = a_Light.outerConeAngle;
    return glslLight;
}

static LightData ConvertLight(Renderer::Impl& a_Renderer, const LightDirectional& a_Light, const MSG::Transform& a_Transform)
{
    GLSL::LightDirectional glslLight {};
    glslLight.commonData = ConvertLightCommonData(LIGHT_TYPE_DIRECTIONAL, a_Light, a_Transform);
    glslLight.halfSize   = a_Light.halfSize;
    glslLight.direction  = a_Transform.GetWorldForward();
    return glslLight;
}

static LightData ConvertLight(Renderer::Impl& a_Renderer, const LightIBL& a_Light, const MSG::Transform& a_Transform)
{
    Component::LightIBLData glslLight {};
    glslLight.commonData             = ConvertLightCommonData(LIGHT_TYPE_IBL, a_Light, a_Transform);
    glslLight.halfSize               = a_Light.halfSize;
    glslLight.irradianceCoefficients = a_Light.irradianceCoefficients;
    glslLight.specular               = std::static_pointer_cast<OGLTextureCubemap>(a_Renderer.LoadTexture(a_Light.specular.texture.get()));
    return glslLight;
}

template <typename SGLight>
static GLSL::Camera GetLightShadowProj(const SGLight&, const MSG::Transform&)
{
    errorFatal("Shadow casting not supported for this type of light");
}

template <>
static GLSL::Camera GetLightShadowProj(const LightSpot& a_SGLight, const MSG::Transform& a_Transform)
{
    CameraProjection proj;
    if (a_SGLight.range == std::numeric_limits<float>::infinity()) {
        CameraProjection::PerspectiveInfinite perspInf;
        perspInf.fov         = a_SGLight.outerConeAngle;
        perspInf.aspectRatio = 1.f;
        perspInf.znear       = 0.001f;
        proj                 = perspInf;
    } else {
        CameraProjection::Perspective persp;
        persp.fov         = a_SGLight.outerConeAngle;
        persp.aspectRatio = 1.f;
        persp.znear       = 0.001f;
        persp.zfar        = a_SGLight.range;
        proj              = persp;
    }
    auto view = glm::inverse(a_Transform.GetWorldTransformMatrix());
    return { .projection = proj, .view = view };
}

static LightShadowData ConvertLightShadow(Renderer::Impl& a_Renderer, const PunctualLight& a_SGLight, const MSG::Transform& a_Transform)
{
    if (!a_SGLight.CastsShadow())
        return {};
    LightShadowData shadow;
    auto shadowSettings = a_SGLight.GetShadowSettings();
    GLenum shadowPf;
    switch (shadowSettings.precision) {
    case LightShadowPrecision::High:
        shadowPf = GL_DEPTH_COMPONENT32;
        break;
    case LightShadowPrecision::Medium:
        shadowPf = GL_DEPTH_COMPONENT24;
        break;
    case LightShadowPrecision::Low:
        shadowPf = GL_DEPTH_COMPONENT16;
        break;
    }
    auto texture       = std::make_shared<OGLTexture2D>(a_Renderer.context, shadowSettings.resolution, shadowSettings.resolution, 1, shadowPf);
    shadow.cast        = true;
    shadow.projection  = std::visit([a_Transform](const auto& a_Light) { return GetLightShadowProj(a_Light, a_Transform); }, a_SGLight);
    shadow.map         = texture;
    shadow.frameBuffer = std::make_shared<OGLFrameBuffer>(a_Renderer.context,
        OGLFrameBufferCreateInfo {
            .defaultSize = { texture->width, texture->height, 1 },
            .depthBuffer = texture,
        });
    return shadow;
}

LightData::LightData(
    Renderer::Impl& a_Renderer,
    const PunctualLight& a_SGLight,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    auto& transform = a_Entity.GetComponent<MSG::Transform>();
    *this           = std::visit([&renderer = a_Renderer, &transform](auto& a_Data) { return ConvertLight(renderer, a_Data, transform); }, a_SGLight);
    shadow          = ConvertLightShadow(a_Renderer, a_SGLight, transform);
}
}
