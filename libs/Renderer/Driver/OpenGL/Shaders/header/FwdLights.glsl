#ifndef FWD_LIGHTS_GLSL
#define FWD_LIGHTS_GLSL

#include <Bindings.glsl>
#include <Camera.glsl>
#include <Lights.glsl>
#include <SampleShadowMap.glsl>
#include <SphericalHarmonics.glsl>

#define FWD_LIGHT_MAX_IBL     SAMPLERS_FWD_IBL_COUNT /** max number of IBL lights for each forward rendered primitive */
#define FWD_LIGHT_MAX_SHADOWS SAMPLERS_FWD_SHADOW_COUNT /** max number of shadow casting lights for each forward rendered primitive */

#ifdef __cplusplus
namespace MSG::Renderer::GLSL {
#endif //__cplusplus
struct FwdIBL {
    uint count;
    uint _padding[3];
    LightIBL lights[FWD_LIGHT_MAX_IBL];
};

struct FwdShadowBase {
    Camera projection;
    LightBase light;
    float blurRadius;
    uint _padding[3];
};
struct FwdShadowPoint {
    Camera projection;
    LightPoint light;
    float blurRadius;
    uint _padding[3];
};
struct FwdShadowSpot {
    Camera projection;
    LightSpot light;
    float blurRadius;
    uint _padding[3];
};
struct FwdShadowDir {
    Camera projection;
    LightDirectional light;
    float blurRadius;
    int cascadeIndex;
    uint _padding[2];
};

struct FwdShadowsBase {
    uint count;
    uint _padding[3];
    FwdShadowBase shadows[FWD_LIGHT_MAX_SHADOWS];
};
struct FwdShadowsPoint {
    uint count;
    uint _padding[3];
    FwdShadowPoint shadows[FWD_LIGHT_MAX_SHADOWS];
};
struct FwdShadowsSpot {
    uint count;
    uint _padding[3];
    FwdShadowSpot shadows[FWD_LIGHT_MAX_SHADOWS];
};
struct FwdShadowsDir {
    uint count;
    uint _padding[3];
    FwdShadowDir shadows[FWD_LIGHT_MAX_SHADOWS];
};
#ifdef __cplusplus
static_assert(sizeof(FwdShadowBase) == sizeof(FwdShadowPoint));
static_assert(sizeof(FwdShadowPoint) == sizeof(FwdShadowSpot));
static_assert(sizeof(FwdShadowSpot) == sizeof(FwdShadowDir));
static_assert(sizeof(FwdShadowBase) % 16 == 0);
}
#endif //__cplusplus

#ifndef __cplusplus
////////////////////////////////////////////////////////////////////////////////
// Image Based Lights
////////////////////////////////////////////////////////////////////////////////
layout(binding = UBO_FWD_IBL) uniform FwdIBLBlock
{
    FwdIBL u_FwdIBL;
};
layout(binding = SAMPLERS_FWD_IBL) uniform samplerCube u_FwdIBLSamplers[FWD_LIGHT_MAX_IBL];

vec3 GetIBLColor(IN(BRDF) a_BRDF, IN(vec2) a_BRDFLutSample, IN(vec3) a_WorldPosition, IN(float) a_Occlusion, IN(vec3) a_N, IN(vec3) a_V, IN(float) a_NdotV)
{
    const vec3 R         = -reflect(a_V, a_N);
    vec3 totalLightColor = vec3(0);
    for (uint lightIndex = 0; lightIndex < u_FwdIBL.count; lightIndex++) {
        const LightIBL light       = u_FwdIBL.lights[lightIndex];
        const vec3 lightSpecular   = sampleLod(u_FwdIBLSamplers[lightIndex], R, pow(a_BRDF.alpha, 1.f / 2.f)).rgb;
        const vec3 lightPosition   = light.commonData.position;
        const vec3 lightColor      = light.commonData.color;
        const float lightIntensity = light.commonData.intensity;
        const vec3 lightMin        = lightPosition - light.halfSize;
        const vec3 lightMax        = lightPosition + light.halfSize;
        if (any(lessThan(a_WorldPosition, lightMin)) || any(greaterThan(a_WorldPosition, lightMax)))
            continue;
        const vec3 F        = F_Schlick(a_BRDF, a_NdotV);
        const vec3 diffuse  = a_BRDF.cDiff * SampleSH(light.irradianceCoefficients, a_N) * a_Occlusion;
        const vec3 specular = lightSpecular * (F * a_BRDFLutSample.x + a_BRDFLutSample.y);
        totalLightColor += (diffuse + specular) * lightColor * lightIntensity;
    }
    return totalLightColor;
}

////////////////////////////////////////////////////////////////////////////////
// Shadow Casting Lights
////////////////////////////////////////////////////////////////////////////////
layout(binding = UBO_FWD_SHADOWS) uniform FwdShadowsBlock
{
    FwdShadowsBase u_FwdShadowsBase;
};
layout(binding = UBO_FWD_SHADOWS) uniform FwdShadowsPointBlock
{
    FwdShadowsPoint u_FwdShadowsPoint;
};
layout(binding = UBO_FWD_SHADOWS) uniform FwdShadowsSpotBlock
{
    FwdShadowsSpot u_FwdShadowsSpot;
};
layout(binding = UBO_FWD_SHADOWS) uniform FwdShadowsDirBlock
{
    FwdShadowsDir u_FwdShadowsDir;
};
layout(binding = SAMPLERS_FWD_SHADOW) uniform sampler2DArrayShadow u_FwdShadowSamplers[FWD_LIGHT_MAX_SHADOWS];

vec3 GetShadowLightColor(IN(BRDF) a_BRDF, IN(vec3) a_WorldPosition, IN(vec3) a_N, IN(vec3) a_V, IN(uint) a_FrameIndex)
{
    const vec3 N               = a_N;
    const vec3 V               = a_V;
    const ivec3 shadowRandBase = ivec3(gl_FragCoord.xy, a_FrameIndex % 8);
    vec3 totalLightColor       = vec3(0);
    for (uint i = 0; i < u_FwdShadowsBase.count; i++) {
        const FwdShadowBase shadowBase = u_FwdShadowsBase.shadows[i];
        const int lightType            = shadowBase.light.commonData.type;
        const vec3 lightPosition       = shadowBase.light.commonData.position;
        const vec3 lightColor          = shadowBase.light.commonData.color;
        const float lightMaxIntensity  = shadowBase.light.commonData.intensity;
        const float lightFalloff       = shadowBase.light.commonData.falloff;
        float lightIntensity           = 0;
        vec3 L                         = vec3(0);
        if (lightType == LIGHT_TYPE_POINT) {
            const FwdShadowPoint shadowPoint = u_FwdShadowsPoint.shadows[i];
            const float lightRange           = shadowPoint.light.range;
            const vec3 LVec                  = lightPosition - a_WorldPosition;
            const float LDist                = length(LVec);
            L                                = normalize(LVec);
            ShadowPointData shadowData;
            shadowData.lightDir         = -L;
            shadowData.lightDist        = LDist;
            shadowData.near             = shadowPoint.projection.zNear;
            shadowData.far              = shadowPoint.projection.zFar;
            shadowData.blurRadius       = shadowPoint.blurRadius;
            shadowData.randBase         = shadowRandBase;
            const float shadowIntensity = SampleShadowMap(u_FwdShadowSamplers[i], shadowData);
            lightIntensity              = PointLightIntensity(LDist, lightRange, lightMaxIntensity, lightFalloff) * shadowIntensity;
        } else if (lightType == LIGHT_TYPE_SPOT) {
            const FwdShadowSpot shadowSpot  = u_FwdShadowsSpot.shadows[i];
            const float lightRange          = shadowSpot.light.range;
            const vec3 lightDir             = shadowSpot.light.direction;
            const float lightInnerConeAngle = shadowSpot.light.innerConeAngle;
            const float lightOuterConeAngle = shadowSpot.light.outerConeAngle;
            const vec3 LVec                 = lightPosition - a_WorldPosition;
            const float LDist               = length(LVec);
            L                               = normalize(LVec);
            ShadowSpotData shadowData;
            shadowData.projection       = shadowSpot.projection.projection * shadowSpot.projection.view;
            shadowData.near             = shadowSpot.projection.zNear;
            shadowData.far              = shadowSpot.projection.zFar;
            shadowData.blurRadius       = shadowSpot.blurRadius;
            shadowData.surfacePosition  = a_WorldPosition;
            shadowData.randBase         = shadowRandBase;
            const float shadowIntensity = SampleShadowMap(u_FwdShadowSamplers[i], shadowData);
            lightIntensity              = PointLightIntensity(LDist, lightRange, lightMaxIntensity, lightFalloff)
                * SpotLightIntensity(L, lightDir, lightInnerConeAngle, lightOuterConeAngle)
                * shadowIntensity;
        } else {
            const FwdShadowDir shadowDir = u_FwdShadowsDir.shadows[i];
            // const vec3 lightMin          = lightPosition - shadowDir.light.halfSize;
            // const vec3 lightMax          = lightPosition + shadowDir.light.halfSize;
            // if (any(lessThan(a_WorldPosition, lightMin)) || any(greaterThan(a_WorldPosition, lightMax)))
            //     continue;
            L = -shadowDir.light.direction;
            ShadowDirData shadowData;
            shadowData.surfacePosition  = a_WorldPosition;
            shadowData.projection       = shadowDir.projection.projection * shadowDir.projection.view;
            shadowData.near             = shadowDir.projection.zNear;
            shadowData.far              = shadowDir.projection.zFar;
            shadowData.blurRadius       = shadowDir.blurRadius;
            shadowData.randBase         = shadowRandBase;
            const float shadowIntensity = SampleShadowMap(u_FwdShadowSamplers[i], shadowData);
            lightIntensity              = lightMaxIntensity * shadowIntensity;
        }
        const float NdotL = saturate(dot(N, L));
        if (NdotL == 0)
            continue;
        const vec3 diffuse            = a_BRDF.cDiff * NdotL;
        const vec3 specular           = GGXSpecular(a_BRDF, N, V, L);
        const vec3 lightParticipation = diffuse + specular;
        totalLightColor += lightParticipation * lightColor * lightIntensity;
    }
    return totalLightColor;
}
#endif //__cplusplus

#endif // FWD_LIGHTS_GLSL