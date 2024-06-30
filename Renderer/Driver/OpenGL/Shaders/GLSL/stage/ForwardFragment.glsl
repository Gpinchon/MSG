#include <MaterialUBO.glsl>
#include <VTFS.glsl>

layout(std430, binding = 0) readonly buffer VTFSLightsBufferSSBO
{
    VTFSLightsBuffer lightsBuffer;
};

layout(std430, binding = 1) readonly buffer VTFSClustersSSBO
{
    VTFSCluster vtfsClusters[];
};

layout(location = 0) in vec3 in_WorldPosition;
layout(location = 1) noperspective in vec3 in_NDCPosition;

out vec4 fragColor;

void main()
{
    uvec3 vtfsClusterIndex    = VTFSClusterIndex(in_NDCPosition);
    uint vtfsClusterIndex1D   = VTFSClusterIndexTo1D(vtfsClusterIndex);
    uint lightCount           = vtfsClusters[vtfsClusterIndex1D].count;
    float totalLightIntensity = 0;
    for (uint i = 0; i < lightCount; i++) {
        uint lightIndex        = vtfsClusters[vtfsClusterIndex1D].index[i];
        vec3 lightPosition     = lightsBuffer.lights[lightIndex].position;
        float lightRange       = lightsBuffer.lights[lightIndex].range;
        float lightDistance    = distance(lightPosition, in_WorldPosition);
        float lightAttenuation = LightAttenuation(lightDistance, lightRange, 1, 0.5);
        totalLightIntensity += clamp(lightAttenuation, 0, 1);
    }
    fragColor.xyz = vec3(totalLightIntensity);
    fragColor.a   = 1;
}