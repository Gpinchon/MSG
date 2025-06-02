#include <MSG/Camera/Projection.hpp>
#include <MSG/Entity/Node.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLTexture2DArray.hpp>
#include <MSG/OGLTextureCube.hpp>
#include <MSG/Renderer/OGL/Components/LightData.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Scene.hpp>
#include <MSG/SphericalHarmonics.hpp>
#include <MSG/Texture.hpp>

#include <GL/glew.h>

#define MIPMAP_COUNT(w, h, d) uint32_t(log2(std::max(std::max(uint32_t(w), uint32_t(h)), uint32_t(d))) + 1)

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

static LightDataBase ConvertLight(Renderer::Impl& a_Renderer, const LightPoint& a_Light, const MSG::Transform& a_Transform)
{
    GLSL::LightPoint glslLight {};
    glslLight.commonData = ConvertLightCommonData(LIGHT_TYPE_POINT, a_Light, a_Transform);
    glslLight.range      = a_Light.range;
    return glslLight;
}

static LightDataBase ConvertLight(Renderer::Impl& a_Renderer, const LightSpot& a_Light, const MSG::Transform& a_Transform)
{
    GLSL::LightSpot glslLight {};
    glslLight.commonData     = ConvertLightCommonData(LIGHT_TYPE_SPOT, a_Light, a_Transform);
    glslLight.range          = a_Light.range;
    glslLight.direction      = a_Transform.GetWorldForward();
    glslLight.innerConeAngle = a_Light.innerConeAngle;
    glslLight.outerConeAngle = a_Light.outerConeAngle;
    return glslLight;
}

static LightDataBase ConvertLight(Renderer::Impl& a_Renderer, const LightDirectional& a_Light, const MSG::Transform& a_Transform)
{
    GLSL::LightDirectional glslLight {};
    glslLight.commonData = ConvertLightCommonData(LIGHT_TYPE_DIRECTIONAL, a_Light, a_Transform);
    glslLight.halfSize   = a_Light.halfSize;
    glslLight.direction  = a_Transform.GetWorldForward();
    return glslLight;
}

static LightDataBase ConvertLight(Renderer::Impl& a_Renderer, const LightIBL& a_Light, const MSG::Transform& a_Transform)
{
    Component::LightIBLData glslLight {};
    glslLight.commonData             = ConvertLightCommonData(LIGHT_TYPE_IBL, a_Light, a_Transform);
    glslLight.halfSize               = a_Light.halfSize;
    glslLight.irradianceCoefficients = a_Light.irradianceCoefficients;
    glslLight.specular               = std::static_pointer_cast<OGLTextureCube>(a_Renderer.LoadTexture(a_Light.specular.texture.get()));
    return glslLight;
}

LightData::LightData(
    Renderer::Impl& a_Renderer,
    ECS::DefaultRegistry& a_Registry,
    const ECS::DefaultRegistry::EntityIDType& a_EntityID)
{
    Update(a_Renderer, a_Registry, a_EntityID);
}

void MSG::Renderer::Component::LightData::Update(
    Renderer::Impl& a_Renderer,
    ECS::DefaultRegistry& a_Registry,
    const ECS::DefaultRegistry::EntityIDType& a_EntityID)
{
    auto& lightData         = a_Registry.GetComponent<MSG::PunctualLight>(a_EntityID);
    auto& transform         = a_Registry.GetComponent<MSG::Transform>(a_EntityID);
    LightDataBase lightBase = std::visit([this, &renderer = a_Renderer, &transform](auto& a_SGData) mutable {
        return ConvertLight(renderer, a_SGData, transform);
    },
        lightData);
    LightDataBase::operator=(lightBase);
    if (!lightData.CastsShadow())
        shadow = {};
    else if (!shadow.has_value())
        shadow = LightShadowData(a_Renderer, lightData, transform);
}

template <typename T>
std::vector<GLSL::Camera> CreateProjBuffer(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const T&)
{
    errorFatal("Shadow casting not available for this type of light");
}

template <>
std::vector<GLSL::Camera> CreateProjBuffer(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const LightDirectional&)
{
    return std::vector<GLSL::Camera>(a_ShadowSettings.cascadeCount);
}

template <>
std::vector<GLSL::Camera> CreateProjBuffer(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const LightSpot&)
{
    return std::vector<GLSL::Camera>(1);
}

template <>
std::vector<GLSL::Camera> CreateProjBuffer(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const LightPoint&)
{
    return std::vector<GLSL::Camera>(6);
}

GLenum GetShadowDepthPixelFormat(const LightShadowPrecision& a_Precision)
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
std::shared_ptr<OGLTexture> CreateTextureDepth(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const T&)
{
    errorFatal("Shadow casting not available for this type of light");
}

template <>
std::shared_ptr<OGLTexture> CreateTextureDepth(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const LightDirectional&)
{
    OGLTexture2DArrayInfo info {
        .width       = a_ShadowSettings.resolution,
        .height      = a_ShadowSettings.resolution,
        .layers      = a_ShadowSettings.cascadeCount,
        .levels      = MIPMAP_COUNT(a_ShadowSettings.resolution, a_ShadowSettings.resolution, 1),
        .sizedFormat = GetShadowDepthPixelFormat(a_ShadowSettings.precision)
    };
    return std::make_shared<OGLTexture2DArray>(a_Ctx, info);
}

template <>
std::shared_ptr<OGLTexture> CreateTextureDepth(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const LightSpot&)
{
    OGLTexture2DArrayInfo info {
        .width       = a_ShadowSettings.resolution,
        .height      = a_ShadowSettings.resolution,
        .layers      = 1,
        .levels      = MIPMAP_COUNT(a_ShadowSettings.resolution, a_ShadowSettings.resolution, 1),
        .sizedFormat = GetShadowDepthPixelFormat(a_ShadowSettings.precision)
    };
    return std::make_shared<OGLTexture2DArray>(a_Ctx, info);
}

template <>
std::shared_ptr<OGLTexture> CreateTextureDepth(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const LightPoint&)
{
    OGLTexture2DArrayInfo info {
        .width       = a_ShadowSettings.resolution,
        .height      = a_ShadowSettings.resolution,
        .layers      = 6,
        .levels      = MIPMAP_COUNT(a_ShadowSettings.resolution, a_ShadowSettings.resolution, 1),
        .sizedFormat = GetShadowDepthPixelFormat(a_ShadowSettings.precision)
    };
    return std::make_shared<OGLTexture2DArray>(a_Ctx, info);
}

LightShadowData::LightShadowData(Renderer::Impl& a_Rdr, const PunctualLight& a_SGLight, const MSG::Transform& a_Transform)
{
    auto shadowSettings = a_SGLight.GetShadowSettings();
    textureDepth        = std::visit([&ctx = a_Rdr.context, &shadowSettings](auto& a_SGLightData) { return CreateTextureDepth(ctx, shadowSettings, a_SGLightData); }, a_SGLight);
    projections         = std::visit([&ctx = a_Rdr.context, &shadowSettings](auto& a_SGLightData) { return CreateProjBuffer(ctx, shadowSettings, a_SGLightData); }, a_SGLight);
    blurRadius          = shadowSettings.blurRadius;
    bias                = shadowSettings.bias;
    for (uint8_t layer = 0u; layer < textureDepth->depth; layer++) {
        frameBuffers.emplace_back(std::make_shared<OGLFrameBuffer>(a_Rdr.context,
            OGLFrameBufferCreateInfo {
                .layered     = textureDepth->depth > 1 ? true : false,
                .defaultSize = { shadowSettings.resolution, shadowSettings.resolution, 1 },
                .depthBuffer = { .layer = layer, .texture = textureDepth },
            }));
    }
}

void LightShadowData::Update(
    Renderer::Impl& a_Renderer,
    ECS::DefaultRegistry& a_Registry,
    const ECS::DefaultRegistry::EntityIDType& a_EntityID,
    const std::vector<SceneShadowViewport>& a_Viewports)
{
    auto& lightData      = a_Registry.GetComponent<PunctualLight>(a_EntityID);
    auto& lightTransform = a_Registry.GetComponent<Transform>(a_EntityID);
    blurRadius           = lightData.GetShadowSettings().blurRadius;
    bias                 = lightData.GetShadowSettings().bias;
    for (uint8_t vI = 0; vI < a_Viewports.size(); vI++) {
        const auto& viewport = a_Viewports.at(vI);
        const float zNear    = viewport.projection.GetZNear();
        const float zFar     = viewport.projection.GetZFar();
        auto& proj           = projections[vI];
        proj.projection      = viewport.projection;
        proj.view            = viewport.viewMatrix;
        proj.position        = lightTransform.GetWorldPosition();
        proj.zNear           = zNear;
        proj.zFar            = zFar == std::numeric_limits<float>::infinity() ? 1000000.f : zFar;
    }
}
}
