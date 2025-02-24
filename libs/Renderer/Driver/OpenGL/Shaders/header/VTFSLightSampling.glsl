#ifndef VTFS_LIGHT_SAMPLING_GLSL
#define VTFS_LIGHT_SAMPLING_GLSL
#ifndef __cplusplus
#include <VTFSInputs.glsl>

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

#endif //__cplusplus
#endif // VTFS_LIGHT_SAMPLING_GLSL
