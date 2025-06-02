#ifndef FWD_LIGHTS_GLSL
#define FWD_LIGHTS_GLSL

#include <Camera.glsl>
#include <SampleShadowMap.glsl>
#include <ShadowData.glsl>

#define SHADOW_MAX_VIEWPORTS 32

#ifndef __cplusplus
////////////////////////////////////////////////////////////////////////////////
// Shadow Casting Lights
////////////////////////////////////////////////////////////////////////////////
layout(binding = SSBO_SHADOW_DATA) readonly buffer ShadowsBlock
{
    ShadowsBase ssbo_ShadowsBase;
};
layout(binding = SSBO_SHADOW_DATA) readonly buffer ShadowsPointBlock
{
    ShadowsPoint ssbo_ShadowsPoint;
};
layout(binding = SSBO_SHADOW_DATA) readonly buffer ShadowsSpotBlock
{
    ShadowsSpot ssbo_ShadowsSpot;
};
layout(binding = SSBO_SHADOW_DATA) readonly buffer ShadowsDirBlock
{
    ShadowsDir ssbo_ShadowsDir;
};
layout(binding = SSBO_SHADOW_VIEWPORTS) readonly buffer ShadowViewports
{
    Camera ssbo_ShadowsViewports[SHADOW_MAX_VIEWPORTS];
};

layout(binding = SAMPLERS_SHADOW) uniform sampler2DArrayShadow u_ShadowSamplers[SAMPLERS_SHADOW_COUNT];

#ifdef BRDF_GLSL
vec3 GetShadowLightColor(IN(BRDF) a_BRDF, IN(vec3) a_WorldPosition, IN(float) a_BlurRadiusOffset, IN(vec3) a_N, IN(vec3) a_V, IN(vec2) a_FragCoord, IN(uint) a_FrameIndex)
{
    const vec3 N         = a_N;
    const vec3 V         = a_V;
    vec3 totalLightColor = vec3(0);
    for (uint i = 0; i < ssbo_ShadowsBase.count; i++) {
        const ShadowBase shadowBase   = ssbo_ShadowsBase.shadows[i];
        const Camera projection       = ssbo_ShadowsViewports[shadowBase.viewportIndex];
        const int lightType           = shadowBase.light.commonData.type;
        const vec3 lightPosition      = shadowBase.light.commonData.position;
        const vec3 lightColor         = shadowBase.light.commonData.color;
        const float lightMaxIntensity = shadowBase.light.commonData.intensity;
        const float lightFalloff      = shadowBase.light.commonData.falloff;
        float lightIntensity          = 0;
        vec3 L                        = vec3(0);
        if (lightType == LIGHT_TYPE_POINT) {
            const ShadowPoint shadowPoint = ssbo_ShadowsPoint.shadows[i];
            const float lightRange        = shadowPoint.light.range;
            const vec3 LVec               = lightPosition - a_WorldPosition;
            const float LDist             = length(LVec);
            L                             = LVec / LDist;
            ShadowPointData shadowData;
            shadowData.lightPosition    = lightPosition;
            shadowData.surfacePosition  = a_WorldPosition;
            shadowData.blurRadius       = shadowPoint.blurRadius;
            shadowData.bias             = shadowPoint.bias;
            shadowData.near             = projection.zNear;
            shadowData.far              = projection.zFar;
            const float shadowIntensity = SampleShadowMap(u_ShadowSamplers[i], shadowData, a_FragCoord, a_FrameIndex);
            lightIntensity              = PointLightIntensity(LDist, lightRange, lightMaxIntensity, lightFalloff) * shadowIntensity;
        } else if (lightType == LIGHT_TYPE_SPOT) {
            const ShadowSpot shadowSpot     = ssbo_ShadowsSpot.shadows[i];
            const float lightRange          = shadowSpot.light.range;
            const vec3 lightDir             = shadowSpot.light.direction;
            const float lightInnerConeAngle = shadowSpot.light.innerConeAngle;
            const float lightOuterConeAngle = shadowSpot.light.outerConeAngle;
            const vec3 LVec                 = lightPosition - a_WorldPosition;
            const float LDist               = length(LVec);
            L                               = LVec / LDist;
            ShadowSpotData shadowData;
            shadowData.lightPosition    = lightPosition;
            shadowData.surfacePosition  = a_WorldPosition;
            shadowData.projection       = projection.projection;
            shadowData.view             = projection.view;
            shadowData.blurRadius       = shadowSpot.blurRadius;
            shadowData.bias             = shadowSpot.bias;
            const float shadowIntensity = SampleShadowMap(u_ShadowSamplers[i], shadowData, a_FragCoord, a_FrameIndex);
            lightIntensity              = PointLightIntensity(LDist, lightRange, lightMaxIntensity, lightFalloff)
                * SpotLightIntensity(L, lightDir, lightInnerConeAngle, lightOuterConeAngle)
                * shadowIntensity;
        } else {
            const ShadowDir shadowDir = ssbo_ShadowsDir.shadows[i];
            // const vec3 lightMin          = lightPosition - shadowDir.light.halfSize;
            // const vec3 lightMax          = lightPosition + shadowDir.light.halfSize;
            // if (any(lessThan(a_WorldPosition, lightMin)) || any(greaterThan(a_WorldPosition, lightMax)))
            //     continue;
            L = -shadowDir.light.direction;
            ShadowDirData shadowData;
            shadowData.lightPosition    = lightPosition;
            shadowData.surfacePosition  = a_WorldPosition;
            shadowData.projection       = projection.projection;
            shadowData.view             = projection.view;
            shadowData.blurRadius       = shadowDir.blurRadius;
            shadowData.bias             = shadowDir.bias;
            const float shadowIntensity = SampleShadowMap(u_ShadowSamplers[i], shadowData, a_FragCoord, a_FrameIndex);
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