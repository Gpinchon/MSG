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

// SHADOW DATA

template <typename SGLight>
static GLSL::Camera GetLightShadowProj(const MSG::Scene&, const SGLight&, const MSG::Transform&)
{
    errorFatal("Shadow casting not supported for this type of light");
}

template <>
static GLSL::Camera GetLightShadowProj(const MSG::Scene& a_Scene, const MSG::LightDirectional& a_SGLight, const MSG::Transform& a_Transform)
{
    const auto position = a_Transform.GetWorldPosition();
    const auto view     = glm::inverse(a_Transform.GetWorldTransformMatrix());
    glm::vec3 halfSize;
    for (auto i = 0u; i < 3; i++) {
        if (a_SGLight.halfSize[i] == std::numeric_limits<float>::infinity())
            halfSize[i] = a_Scene.GetBoundingVolume().halfSize[i];
        else
            halfSize[i] = a_SGLight.halfSize[i];
    }
    const auto AABB = view * BoundingVolume(position, halfSize);
    auto minOrtho   = AABB.Min();
    auto maxOrtho   = AABB.Max();
    const auto proj = CameraProjection {
        CameraProjectionOrthographic {
            .xmag  = (maxOrtho.x - minOrtho.x) * 0.5f,
            .ymag  = (maxOrtho.y - minOrtho.y) * 0.5f,
            .znear = minOrtho.z,
            .zfar  = (maxOrtho.z - minOrtho.z),
        }
    };
    return { .projection = proj, .view = view, .zNear = minOrtho.z, .zFar = maxOrtho.z, .position = position };
}

template <>
static GLSL::Camera GetLightShadowProj(const MSG::Scene& a_Scene, const MSG::LightSpot& a_SGLight, const MSG::Transform& a_Transform)
{
    CameraProjection proj;
    const bool isInfinite = a_SGLight.range == std::numeric_limits<float>::infinity();
    const float zNear     = 0.001f;
    const float zFar      = isInfinite ? 1000000.f : a_SGLight.range;
    if (isInfinite) {
        CameraProjectionPerspectiveInfinite perspInf;
        perspInf.fov         = a_SGLight.outerConeAngle * 2.f * (180.f / M_PIf);
        perspInf.aspectRatio = 1.f;
        perspInf.znear       = zNear;
        proj                 = perspInf;
    } else {
        CameraProjectionPerspective persp;
        persp.fov         = a_SGLight.outerConeAngle * 2.f * (180.f / M_PIf);
        persp.aspectRatio = 1.f;
        persp.znear       = zNear;
        persp.zfar        = zFar;
        proj              = persp;
    }
    auto view = glm::inverse(a_Transform.GetWorldTransformMatrix());
    return { .projection = proj, .view = view, .zNear = zNear, .zFar = zFar, .position = a_Transform.GetWorldPosition() };
}

LightShadowData::LightShadowData(Renderer::Impl& a_Rdr, const PunctualLight& a_SGLight, const MSG::Transform& a_Transform)
    : cast(a_SGLight.CastsShadow())
    , projBuffer(std::make_shared<OGLTypedBuffer<GLSL::Camera>>(a_Rdr.context, std::visit([&a_Rdr, &a_Transform](const auto& a_Light) { return GetLightShadowProj(*a_Rdr.activeScene, a_Light, a_Transform); }, a_SGLight)))
{
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
    auto texture2D = std::make_shared<OGLTexture2D>(a_Rdr.context, shadowSettings.resolution, shadowSettings.resolution, 1, shadowPf);
    texture        = texture2D;
    frameBuffer    = std::make_shared<OGLFrameBuffer>(a_Rdr.context,
           OGLFrameBufferCreateInfo {
               .defaultSize = { texture2D->width, texture2D->height, 1 },
               .depthBuffer = texture2D,
        });
}
}
