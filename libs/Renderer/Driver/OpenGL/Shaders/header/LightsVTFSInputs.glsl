#ifndef VTFS_INPUTS_GLSL
#define VTFS_INPUTS_GLSL
#ifndef __cplusplus
#include <Bindings.glsl>
#include <LightsVTFS.glsl>

layout(std430, binding = SSBO_VTFS_LIGHTS) readonly buffer VTFSLightsBufferSSBO
{
    LightBase lightBase[VTFS_BUFFER_MAX];
};
layout(std430, binding = SSBO_VTFS_LIGHTS) readonly buffer VTFSLightPointBufferSSBO
{
    LightPoint lightPoint[VTFS_BUFFER_MAX];
};
layout(std430, binding = SSBO_VTFS_LIGHTS) readonly buffer VTFSLightSpotBufferSSBO
{
    LightSpot lightSpot[VTFS_BUFFER_MAX];
};
layout(std430, binding = SSBO_VTFS_LIGHTS) readonly buffer VTFSLightDirBufferSSBO
{
    LightDirectional lightDirectional[VTFS_BUFFER_MAX];
};

layout(std430, binding = SSBO_VTFS_CLUSTERS) readonly buffer VTFSClustersSSBO
{
    VTFSCluster vtfsClusters[VTFS_CLUSTER_COUNT];
};

#ifdef BRDF_GLSL
vec3 GetVTFSLightColor(IN(BRDF) a_BRDF, IN(vec3) a_WorldPosition, IN(vec3) a_NDCPosition, IN(vec3) a_N, IN(vec3) a_V)
{
    const vec3 N                  = a_N;
    const vec3 V                  = a_V;
    const uvec3 vtfsClusterIndex  = VTFSClusterIndex(a_NDCPosition);
    const uint vtfsClusterIndex1D = VTFSClusterIndexTo1D(vtfsClusterIndex);
    const uint lightCount         = vtfsClusters[vtfsClusterIndex1D].count;
    vec3 totalLightColor          = vec3(0);
    for (uint i = 0; i < lightCount; i++) {
        const uint lightIndex         = vtfsClusters[vtfsClusterIndex1D].index[i];
        const int lightType           = lightBase[lightIndex].commonData.type;
        const vec3 lightPosition      = lightBase[lightIndex].commonData.position;
        const vec3 lightColor         = lightBase[lightIndex].commonData.color;
        const float lightMaxIntensity = lightBase[lightIndex].commonData.intensity;
        const float lightFalloff      = lightBase[lightIndex].commonData.falloff;
        float lightIntensity          = 0;
        vec3 L                        = vec3(0);
        if (lightType == LIGHT_TYPE_POINT || lightType == LIGHT_TYPE_SPOT) {
            const vec3 LVec   = lightPosition - a_WorldPosition;
            const float LDist = length(LVec);
            L                 = normalize(LVec);
            lightIntensity    = PointLightIntensity(LDist, lightPoint[lightIndex].range, lightMaxIntensity, lightFalloff);
            if (lightType == LIGHT_TYPE_SPOT) {
                const vec3 lightDir             = lightSpot[lightIndex].direction;
                const float lightInnerConeAngle = lightSpot[lightIndex].innerConeAngle;
                const float lightOuterConeAngle = lightSpot[lightIndex].outerConeAngle;
                lightIntensity *= SpotLightIntensity(L, lightDir, lightInnerConeAngle, lightOuterConeAngle);
            }
        } else {
            L              = -lightDirectional[lightIndex].direction;
            lightIntensity = lightMaxIntensity;
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
#endif // VTFS_INPUTS_GLSL