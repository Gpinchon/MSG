#include <MSG/Camera/Projection.hpp>
#include <MSG/Entity/Node.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLTextureCubemap.hpp>
#include <MSG/Renderer/OGL/Components/LightData.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Scene.hpp>
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

LightData::LightData(
    Renderer::Impl& a_Renderer,
    const PunctualLight& a_SGLight,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    auto& transform = a_Entity.GetComponent<MSG::Transform>();
    *this           = std::visit([&renderer = a_Renderer, &transform](auto& a_Data) { return ConvertLight(renderer, a_Data, transform); }, a_SGLight);
    if (a_SGLight.CastsShadow())
        shadow = LightShadowData(a_Renderer, a_SGLight, transform);
}

template <typename T>
std::shared_ptr<OGLTypedBufferArray<GLSL::Camera>> CreateProjBuffer(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const T&)
{
    errorFatal("Shadow casting not available for this type of light");
}

template <>
std::shared_ptr<OGLTypedBufferArray<GLSL::Camera>> CreateProjBuffer(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const LightDirectional&)
{
    return std::make_shared<OGLTypedBufferArray<GLSL::Camera>>(a_Ctx, a_ShadowSettings.cascadeCount);
}

template <>
std::shared_ptr<OGLTypedBufferArray<GLSL::Camera>> CreateProjBuffer(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const LightSpot&)
{
    return std::make_shared<OGLTypedBufferArray<GLSL::Camera>>(a_Ctx, 1);
}

template <>
std::shared_ptr<OGLTypedBufferArray<GLSL::Camera>> CreateProjBuffer(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const LightPoint&)
{
    return std::make_shared<OGLTypedBufferArray<GLSL::Camera>>(a_Ctx, 6);
}

GLenum GetShadowPixelFormat(const LightShadowPrecision& a_Precision)
{
    switch (a_Precision) {
    case LightShadowPrecision::High:
        return GL_DEPTH_COMPONENT32;
    case LightShadowPrecision::Medium:
        return GL_DEPTH_COMPONENT24;
    case LightShadowPrecision::Low:
        return GL_DEPTH_COMPONENT16;
    }
    return GL_NONE;
}

template <typename T>
std::shared_ptr<OGLTexture> CreateTexture(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const T&)
{
    errorFatal("Shadow casting not available for this type of light");
}

template <>
std::shared_ptr<OGLTexture> CreateTexture(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const LightDirectional&)
{
    return std::make_shared<OGLTexture2D>(a_Ctx,
        a_ShadowSettings.resolution, a_ShadowSettings.resolution, 1,
        GetShadowPixelFormat(a_ShadowSettings.precision));
}

template <>
std::shared_ptr<OGLTexture> CreateTexture(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const LightSpot&)
{
    return std::make_shared<OGLTexture2D>(a_Ctx,
        a_ShadowSettings.resolution, a_ShadowSettings.resolution, 1,
        GetShadowPixelFormat(a_ShadowSettings.precision));
}

template <>
std::shared_ptr<OGLTexture> CreateTexture(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const LightPoint&)
{
    return std::make_shared<OGLTextureCubemap>(a_Ctx,
        a_ShadowSettings.resolution, a_ShadowSettings.resolution, 1,
        GetShadowPixelFormat(a_ShadowSettings.precision));
}

LightShadowData::LightShadowData(Renderer::Impl& a_Rdr, const PunctualLight& a_SGLight, const MSG::Transform& a_Transform)
    : cast(a_SGLight.CastsShadow())
{
    auto shadowSettings = a_SGLight.GetShadowSettings();
    texture             = std::visit([&ctx = a_Rdr.context, &shadowSettings](auto& a_SGLightData) { return CreateTexture(ctx, shadowSettings, a_SGLightData); }, a_SGLight);
    projBuffer          = std::visit([&ctx = a_Rdr.context, &shadowSettings](auto& a_SGLightData) { return CreateProjBuffer(ctx, shadowSettings, a_SGLightData); }, a_SGLight);
    for (uint8_t layer = 0u; layer < texture->depth; layer++) {
        frameBuffers.emplace_back(std::make_shared<OGLFrameBuffer>(a_Rdr.context,
            OGLFrameBufferCreateInfo {
                .defaultSize = { shadowSettings.resolution, shadowSettings.resolution, 1 },
                .depthBuffer = { .layer = layer, .texture = texture },
            }));
    }
}
}
