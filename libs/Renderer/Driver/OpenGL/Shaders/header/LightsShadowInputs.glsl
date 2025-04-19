#ifndef FWD_LIGHTS_GLSL
#define FWD_LIGHTS_GLSL

#include <Bindings.glsl>
#include <Camera.glsl>
#include <Lights.glsl>
#include <SampleShadowMap.glsl>

#ifdef __cplusplus
namespace MSG::Renderer::GLSL {
#endif //__cplusplus
struct ShadowBase {
    Camera projection;
    LightBase light;
    float blurRadius;
    uint _padding[3];
};
struct ShadowPoint {
    Camera projection;
    LightPoint light;
    float blurRadius;
    uint _padding[3];
};
struct ShadowSpot {
    Camera projection;
    LightSpot light;
    float blurRadius;
    uint _padding[3];
};
struct ShadowDir {
    Camera projection;
    LightDirectional light;
    float blurRadius;
    int cascadeIndex;
    uint _padding[2];
};

struct ShadowsBase {
    uint count;
    uint _padding[3];
    ShadowBase shadows[SAMPLERS_SHADOW_COUNT];
};
struct ShadowsPoint {
    uint count;
    uint _padding[3];
    ShadowPoint shadows[SAMPLERS_SHADOW_COUNT];
};
struct ShadowsSpot {
    uint count;
    uint _padding[3];
    ShadowSpot shadows[SAMPLERS_SHADOW_COUNT];
};
struct ShadowsDir {
    uint count;
    uint _padding[3];
    ShadowDir shadows[SAMPLERS_SHADOW_COUNT];
};
#ifdef __cplusplus
static_assert(sizeof(ShadowBase) == sizeof(ShadowPoint));
static_assert(sizeof(ShadowPoint) == sizeof(ShadowSpot));
static_assert(sizeof(ShadowSpot) == sizeof(ShadowDir));
static_assert(sizeof(ShadowBase) % 16 == 0);
}
#endif //__cplusplus

#ifndef __cplusplus
////////////////////////////////////////////////////////////////////////////////
// Shadow Casting Lights
////////////////////////////////////////////////////////////////////////////////
layout(binding = UBO_FWD_SHADOWS) uniform ShadowsBlock
{
    ShadowsBase u_ShadowsBase;
};
layout(binding = UBO_FWD_SHADOWS) uniform ShadowsPointBlock
{
    ShadowsPoint u_ShadowsPoint;
};
layout(binding = UBO_FWD_SHADOWS) uniform ShadowsSpotBlock
{
    ShadowsSpot u_ShadowsSpot;
};
layout(binding = UBO_FWD_SHADOWS) uniform ShadowsDirBlock
{
    ShadowsDir u_ShadowsDir;
};
layout(binding = SAMPLERS_SHADOW) uniform sampler2DArrayShadow u_ShadowSamplers[SAMPLERS_SHADOW_COUNT];

#ifdef BRDF_GLSL
vec3 GetShadowLightColor(IN(BRDF) a_BRDF, IN(vec3) a_WorldPosition, IN(float) a_BlurRadiusOffset, IN(vec3) a_N, IN(vec3) a_V, IN(uint) a_FrameIndex)
{
    const vec3 N               = a_N;
    const vec3 V               = a_V;
    const ivec3 shadowRandBase = ivec3(gl_FragCoord.xy, a_FrameIndex % 8);
    vec3 totalLightColor       = vec3(0);
    for (uint i = 0; i < u_ShadowsBase.count; i++) {
        const ShadowBase shadowBase   = u_ShadowsBase.shadows[i];
        const int lightType           = shadowBase.light.commonData.type;
        const vec3 lightPosition      = shadowBase.light.commonData.position;
        const vec3 lightColor         = shadowBase.light.commonData.color;
        const float lightMaxIntensity = shadowBase.light.commonData.intensity;
        const float lightFalloff      = shadowBase.light.commonData.falloff;
        float lightIntensity          = 0;
        vec3 L                        = vec3(0);
        if (lightType == LIGHT_TYPE_POINT) {
            const ShadowPoint shadowPoint = u_ShadowsPoint.shadows[i];
            const float lightRange        = shadowPoint.light.range;
            const vec3 LVec               = lightPosition - a_WorldPosition;
            const float LDist             = length(LVec);
            L                             = normalize(LVec);
            ShadowPointData shadowData;
            shadowData.lightDir         = -L;
            shadowData.lightDist        = LDist;
            shadowData.near             = shadowPoint.projection.zNear;
            shadowData.far              = shadowPoint.projection.zFar;
            shadowData.blurRadius       = shadowPoint.blurRadius + shadowPoint.blurRadius * a_BlurRadiusOffset;
            shadowData.randBase         = shadowRandBase;
            const float shadowIntensity = SampleShadowMap(u_ShadowSamplers[i], shadowData);
            lightIntensity              = PointLightIntensity(LDist, lightRange, lightMaxIntensity, lightFalloff) * shadowIntensity;
        } else if (lightType == LIGHT_TYPE_SPOT) {
            const ShadowSpot shadowSpot     = u_ShadowsSpot.shadows[i];
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
            shadowData.blurRadius       = shadowSpot.blurRadius + shadowSpot.blurRadius * a_BlurRadiusOffset;
            shadowData.surfacePosition  = a_WorldPosition;
            shadowData.randBase         = shadowRandBase;
            const float shadowIntensity = SampleShadowMap(u_ShadowSamplers[i], shadowData);
            lightIntensity              = PointLightIntensity(LDist, lightRange, lightMaxIntensity, lightFalloff)
                * SpotLightIntensity(L, lightDir, lightInnerConeAngle, lightOuterConeAngle)
                * shadowIntensity;
        } else {
            const ShadowDir shadowDir = u_ShadowsDir.shadows[i];
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
            shadowData.blurRadius       = shadowDir.blurRadius + shadowDir.blurRadius * a_BlurRadiusOffset;
            shadowData.randBase         = shadowRandBase;
            const float shadowIntensity = SampleShadowMap(u_ShadowSamplers[i], shadowData);
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
#endif // BRDF_GLSL
#endif //__cplusplus

#endif // FWD_LIGHTS_GLSL