#include <MaterialUBO.glsl>
#include <VTFS.glsl>

layout(std430, binding = 0) readonly buffer VTFSLightsBufferSSBO
{
    LightBase lightBase[VTFS_BUFFER_MAX];
};
layout(std430, binding = 0) readonly buffer VTFSLightSpotBufferSSBO
{
    LightSpot lightSpot[VTFS_BUFFER_MAX];
};
layout(std430, binding = 0) readonly buffer VTFSLightDirBufferSSBO
{
    LightDirectional lightDirectional[VTFS_BUFFER_MAX];
};

layout(std430, binding = 1) readonly buffer VTFSClustersSSBO
{
    VTFSCluster vtfsClusters[];
};

layout(location = 0) in vec3 in_WorldPosition;
layout(location = 1) in vec3 in_WorldNormal;
layout(location = 10) noperspective in vec3 in_NDCPosition;

out vec4 fragColor;

INLINE vec3 SampleLight(
    IN(vec3) a_WorldPosition,
    IN(vec3) a_WorldNormal,
    IN(uint) a_LightIndex)
{
    const int lightType        = lightBase[a_LightIndex].commonData.type;
    const vec3 lightPosition   = lightBase[a_LightIndex].commonData.position;
    const vec3 lightColor      = lightBase[a_LightIndex].commonData.color;
    const float lightRange     = lightBase[a_LightIndex].commonData.range;
    const float lightIntensity = lightBase[a_LightIndex].commonData.intensity;
    const float lightFalloff   = lightBase[a_LightIndex].commonData.falloff;
    const vec3 lightVec        = (lightPosition - a_WorldPosition);
    const vec3 lightVecNorm    = normalize(lightVec);
    const float lightDot       = max(dot(a_WorldNormal, lightVecNorm), 0);

    float lightAttenuation = 0;
    if (lightType == LIGHT_TYPE_POINT) {
        const float lightDistance = length(lightVec);
        lightAttenuation          = PointLightAttenuation(lightDistance, lightRange, lightIntensity, lightFalloff);
    } else if (lightType == LIGHT_TYPE_SPOT) {
        const float lightDistance         = length(lightVec);
        const vec3 lightDir               = lightSpot[a_LightIndex].direction;
        const float lightInnerConeAngle   = lightSpot[a_LightIndex].innerConeAngle;
        const float lightOuterConeAngle   = lightSpot[a_LightIndex].outerConeAngle;
        const float lightSpotAttenuation  = SpotLightAttenuation(lightVecNorm, lightDir, lightInnerConeAngle, lightOuterConeAngle);
        const float lightPointAttenuation = PointLightAttenuation(lightDistance, lightRange, lightIntensity, lightFalloff);
        lightAttenuation                  = lightSpotAttenuation * lightPointAttenuation;
    }
    return lightDot * lightColor * lightAttenuation;
}

void main()
{
    uvec3 vtfsClusterIndex  = VTFSClusterIndex(in_NDCPosition);
    uint vtfsClusterIndex1D = VTFSClusterIndexTo1D(vtfsClusterIndex);
    uint lightCount         = vtfsClusters[vtfsClusterIndex1D].count;
    vec3 totalLightColor    = vec3(0);
    for (uint i = 0; i < lightCount; i++) {
        totalLightColor += SampleLight(
            in_WorldPosition,
            in_WorldNormal,
            vtfsClusters[vtfsClusterIndex1D].index[i]);
    }
    fragColor.xyz = totalLightColor;
    fragColor.a   = 1;
}