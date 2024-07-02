#include <MaterialUBO.glsl>
#include <VTFS.glsl>

layout(std430, binding = 0) readonly buffer VTFSLightsBufferSSBO
{
    uint count;
    uint _padding[3];
    LightBase lights[VTFS_BUFFER_MAX];
}
lightsBuffer;

layout(std430, binding = 1) readonly buffer VTFSClustersSSBO
{
    VTFSCluster vtfsClusters[];
};

layout(location = 0) in vec3 in_WorldPosition;
layout(location = 1) in vec3 in_WorldNormal;
layout(location = 10) noperspective in vec3 in_NDCPosition;

out vec4 fragColor;

void main()
{
    uvec3 vtfsClusterIndex  = VTFSClusterIndex(in_NDCPosition);
    uint vtfsClusterIndex1D = VTFSClusterIndexTo1D(vtfsClusterIndex);
    uint lightCount         = vtfsClusters[vtfsClusterIndex1D].count;
    vec3 totalLightColor    = vec3(0);
    for (uint i = 0; i < lightCount; i++) {
        const uint lightIndex        = vtfsClusters[vtfsClusterIndex1D].index[i];
        const vec3 lightPosition     = lightsBuffer.lights[lightIndex].commonData.position;
        const vec3 lightColor        = lightsBuffer.lights[lightIndex].commonData.color;
        const float lightRange       = lightsBuffer.lights[lightIndex].commonData.range;
        const float lightIntensity   = lightsBuffer.lights[lightIndex].commonData.intensity;
        const float lightFalloff     = lightsBuffer.lights[lightIndex].commonData.falloff;
        const float lightDistance    = distance(lightPosition, in_WorldPosition);
        const float lightAttenuation = LightAttenuation(
            lightDistance, lightRange, lightIntensity, lightFalloff);
        const vec3 lightDir  = normalize(lightPosition - in_WorldPosition);
        const float lightDot = dot(in_WorldNormal, lightDir);
        totalLightColor += clamp(lightAttenuation, 0, 1) * lightColor * lightDot;
    }
    fragColor.xyz = totalLightColor;
    fragColor.a   = 1;
}