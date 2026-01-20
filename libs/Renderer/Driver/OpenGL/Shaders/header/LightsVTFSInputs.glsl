#ifndef VTFS_INPUTS_GLSL
#define VTFS_INPUTS_GLSL

/**
 *  define VTFS_IGNORE_IBL to 1 to ignore image based lights
 *  define VTFS_IGNORE_SHADOWS to 1 to ignore shadow casting lights
 *  define VTFS_IGNORE_NON_SHADOWS to 1 to ignore non shadow casting lights
 */
#ifndef __cplusplus
#include <Bindings.glsl>
#include <Camera.glsl>
#include <LightsIBL.glsl>
#include <LightsVTFS.glsl>
#include <SampleShadowMap.glsl>

layout(std430, binding = SSBO_VTFS_LIGHTS) readonly buffer VTFSLightsSSBO
{
    LightBase ssbo_LightBase[VTFS_BUFFER_MAX];
};
layout(std430, binding = SSBO_VTFS_LIGHTS) readonly buffer VTFSLightPointSSBO
{
    LightPoint ssbo_LightPoint[VTFS_BUFFER_MAX];
};
layout(std430, binding = SSBO_VTFS_LIGHTS) readonly buffer VTFSLightSpotSSBO
{
    LightSpot ssbo_LightSpot[VTFS_BUFFER_MAX];
};
layout(std430, binding = SSBO_VTFS_LIGHTS) readonly buffer VTFSLightDirSSBO
{
    LightDirectional ssbo_LightDirectional[VTFS_BUFFER_MAX];
};
layout(std430, binding = SSBO_VTFS_LIGHTS) readonly buffer VTFSLightIBLIndexSSBO
{
    LightIBLIndex ssbo_LightIBLIndex[VTFS_BUFFER_MAX];
};
layout(std430, binding = SSBO_IBL) readonly buffer VTFSLightIBLSSBO
{
    LightIBL ssbo_LightIBL[];
};

layout(std430, binding = SSBO_VTFS_CLUSTERS) readonly buffer VTFSClustersSSBO
{
    VTFSCluster ssbo_VTFSClusters[VTFS_CLUSTER_COUNT];
};

layout(std430, binding = SSBO_SHADOW_CASTERS) readonly buffer VTFSShadowCastersSSBO
{
    ShadowCaster ssbo_shadowCasters[];
};

layout(std430, binding = SSBO_SHADOW_VIEWPORTS) readonly buffer VTFSShadowViewportsSSBO
{
    Camera ssbo_shadowViewports[];
};

#ifdef BRDF_GLSL
struct VTFSSampleParameters {
    BRDF brdf;
    vec2 brdfLutSample; // only required for IBLs
    vec3 worldPosition;
    vec3 worldNormal;
    vec3 worldView;
    float normalDotView;
    vec3 NDCPosition;
    vec2 fragCoord;
    uint frameIndex;
};

/** @brief we use this method because we want to cancel normal maps effect for this peculiar case */
vec3 GetWorldNormalFromPosition(IN(vec3) a_WorldPosition)
{
    vec3 X = dFdx(a_WorldPosition);
    vec3 Y = dFdy(a_WorldPosition);
    return normalize(cross(X, Y));
}

float GetVTFSShadowFactorPoint(
    IN(uint) a_LightIndex,
    IN(vec3) a_LightDir,
    IN(VTFSSampleParameters) a_Params)
{
    int casterIndex = ssbo_LightBase[a_LightIndex].commonData.shadowCasterIndex;
    if (casterIndex == -1)
        return 1.f;
    ShadowCaster caster = ssbo_shadowCasters[casterIndex];
    uint viewportIndex  = caster.viewportIndex;
    Camera viewport     = ssbo_shadowViewports[viewportIndex];
    vec3 lightPosition  = ssbo_LightBase[a_LightIndex].commonData.position;
    vec3 surfaceN       = GetWorldNormalFromPosition(a_Params.worldPosition);
    vec3 normalOffset   = surfaceN * (1 - saturate(dot(a_LightDir, surfaceN)));
    ShadowPointData shadowData;
    shadowData.lightPosition   = lightPosition;
    shadowData.surfacePosition = a_Params.worldPosition + normalOffset * caster.normalBias;
    shadowData.blurRadius      = caster.blurRadius;
    shadowData.minDepth        = caster.minDepth;
    shadowData.maxDepth        = caster.maxDepth;
    shadowData.near            = viewport.zNear;
    shadowData.far             = viewport.zFar;
    return SampleShadowMap(caster.samplerHandle, shadowData, a_Params.fragCoord, a_Params.frameIndex);
}

float GetVTFSShadowFactor(
    IN(uint) a_LightIndex,
    IN(vec3) a_LightDir,
    IN(VTFSSampleParameters) a_Params)
{
    int casterIndex = ssbo_LightBase[a_LightIndex].commonData.shadowCasterIndex;
    if (casterIndex == -1)
        return 1.f;
    ShadowCaster caster = ssbo_shadowCasters[casterIndex];
    uint viewportIndex  = caster.viewportIndex;
    Camera viewport     = ssbo_shadowViewports[viewportIndex];
    vec3 lightPosition  = ssbo_LightBase[a_LightIndex].commonData.position;
    vec3 surfaceN       = GetWorldNormalFromPosition(a_Params.worldPosition);
    vec3 normalOffset   = surfaceN * (1 - saturate(dot(a_LightDir, surfaceN)));
    ShadowData shadowData;
    shadowData.lightPosition   = lightPosition;
    shadowData.surfacePosition = a_Params.worldPosition + normalOffset * caster.normalBias;
    shadowData.projection      = viewport.projection;
    shadowData.view            = viewport.view;
    shadowData.blurRadius      = caster.blurRadius;
    shadowData.minDepth        = caster.minDepth;
    shadowData.maxDepth        = caster.maxDepth;
    return SampleShadowMap(caster.samplerHandle, shadowData, a_Params.fragCoord, a_Params.frameIndex);
}

vec3 GetVTFSIBLColor(IN(VTFSSampleParameters) a_Params)
{
    const uvec3 vtfsClusterIndex  = VTFSClusterIndex(a_Params.NDCPosition);
    const uint vtfsClusterIndex1D = VTFSClusterIndexTo1D(vtfsClusterIndex);
    const uint lightCount         = ssbo_VTFSClusters[vtfsClusterIndex1D].count;
    vec3 totalLightColor          = vec3(0);
    for (uint i = 0; i < lightCount; i++) {
        const uint lightIndex         = ssbo_VTFSClusters[vtfsClusterIndex1D].index[i];
        const int lightType           = ssbo_LightBase[lightIndex].commonData.type;
        const vec3 lightPosition      = ssbo_LightBase[lightIndex].commonData.position;
        const vec3 lightColor         = ssbo_LightBase[lightIndex].commonData.color;
        const float lightMaxIntensity = ssbo_LightBase[lightIndex].commonData.intensity;
        const float lightFalloff      = ssbo_LightBase[lightIndex].commonData.falloff;
        float lightIntensity          = 0;
        vec3 L                        = vec3(0);
        if (lightType == LIGHT_TYPE_IBL) {
            uint lightIBLIndex = ssbo_LightIBLIndex[lightIndex].index;
            IBLSampleParameters params;
            params.light         = ssbo_LightIBL[lightIBLIndex];
            params.brdf          = a_Params.brdf;
            params.brdfLutSample = a_Params.brdfLutSample;
            params.worldPosition = a_Params.worldPosition;
            params.worldNormal   = a_Params.worldNormal;
            params.worldView     = a_Params.worldView;
            params.normalDotView = a_Params.normalDotView;
            totalLightColor += GetIBLColor(params);
        }
    }
    return totalLightColor;
}

vec3 GetVTFSLightColor(IN(VTFSSampleParameters) a_Params)
{
    uvec3 vtfsClusterIndex  = VTFSClusterIndex(a_Params.NDCPosition);
    uint vtfsClusterIndex1D = VTFSClusterIndexTo1D(vtfsClusterIndex);
    uint lightCount         = ssbo_VTFSClusters[vtfsClusterIndex1D].count;
    vec3 totalLightColor    = vec3(0);
    for (uint i = 0; i < lightCount; i++) {
        uint lightIndex = ssbo_VTFSClusters[vtfsClusterIndex1D].index[i];
        int lightType   = ssbo_LightBase[lightIndex].commonData.type;
#if VTFS_IGNORE_IBL
        if (lightType == LIGHT_TYPE_IBL)
            continue;
#endif
        if (lightType == LIGHT_TYPE_IBL) {
            uint lightIBLIndex = ssbo_LightIBLIndex[lightIndex].index;
            IBLSampleParameters params;
            params.light         = ssbo_LightIBL[lightIBLIndex];
            params.brdf          = a_Params.brdf;
            params.brdfLutSample = a_Params.brdfLutSample;
            params.worldPosition = a_Params.worldPosition;
            params.worldNormal   = a_Params.worldNormal;
            params.worldView     = a_Params.worldView;
            params.normalDotView = a_Params.normalDotView;
            totalLightColor += GetIBLColor(params);
        } else {
#if VTFS_IGNORE_SHADOWS || VTFS_IGNORE_NON_SHADOWS
            bool castsShadow = ssbo_LightBase[lightIndex].commonData.shadowCasterIndex != -1;
#endif
#if VTFS_IGNORE_SHADOWS
            if (castsShadow)
                continue;
#endif
#if VTFS_IGNORE_NON_SHADOWS
            if (!castsShadow)
                continue;
#endif
            vec3 lightPosition      = ssbo_LightBase[lightIndex].commonData.position;
            vec3 lightColor         = ssbo_LightBase[lightIndex].commonData.color;
            float lightMaxIntensity = ssbo_LightBase[lightIndex].commonData.intensity;
            float lightFalloff      = ssbo_LightBase[lightIndex].commonData.falloff;
            float lightIntensity    = 0;
            vec3 L                  = vec3(0);
            if (lightType == LIGHT_TYPE_POINT) {
                LightPoint lightPoint = ssbo_LightPoint[lightIndex];
                vec3 LVec             = lightPosition - a_Params.worldPosition;
                float LDist           = length(LVec);
                L                     = normalize(LVec);
                float shadowFactor    = GetVTFSShadowFactorPoint(lightIndex, L, a_Params);
                lightIntensity        = PointLightIntensity(LDist, lightPoint.range, lightMaxIntensity, lightFalloff);
                lightIntensity        = lightIntensity * shadowFactor;
            } else if (lightType == LIGHT_TYPE_SPOT) {
                LightSpot lightSpot       = ssbo_LightSpot[lightIndex];
                vec3 LVec                 = lightPosition - a_Params.worldPosition;
                float LDist               = length(LVec);
                L                         = normalize(LVec);
                vec3 lightDir             = lightSpot.direction;
                float lightInnerConeAngle = lightSpot.innerConeAngle;
                float lightOuterConeAngle = lightSpot.outerConeAngle;
                float shadowFactor        = GetVTFSShadowFactor(lightIndex, L, a_Params);
                lightIntensity            = PointLightIntensity(LDist, lightSpot.range, lightMaxIntensity, lightFalloff);
                lightIntensity            = lightIntensity * SpotLightIntensity(L, lightDir, lightInnerConeAngle, lightOuterConeAngle);
                lightIntensity            = lightIntensity * shadowFactor;
            } else {
                L                  = -ssbo_LightDirectional[lightIndex].direction;
                float shadowFactor = GetVTFSShadowFactor(lightIndex, L, a_Params);
                lightIntensity     = lightMaxIntensity;
                lightIntensity     = lightIntensity * shadowFactor;
            }
            float NdotL = saturate(dot(a_Params.worldNormal, L));
            if (NdotL == 0)
                continue;
            vec3 diffuse            = a_Params.brdf.cDiff * NdotL;
            vec3 specular           = GGXSpecular(a_Params.brdf, a_Params.worldNormal, a_Params.worldView, L);
            vec3 lightParticipation = diffuse + specular;
            totalLightColor += lightParticipation * lightColor * lightIntensity;
        }
    }
    return totalLightColor;
}
#endif // BRDF_GLSL
#endif //__cplusplus
#endif // VTFS_INPUTS_GLSL