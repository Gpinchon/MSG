#include <Lights.glsl>
#include <MaterialUBO.glsl>

layout(std430, binding = 0) readonly buffer LightsBufferSSBO
{
    LightsBuffer lightsBuffer;
};

layout(std430, binding = 1) readonly buffer LightClustersSSBO
{
    LightCluster lightClusters[];
};

layout(location = 0) in vec3 in_WorldPosition;
layout(location = 1) noperspective in vec3 in_NDCPosition;

out vec4 fragColor;

void main()
{
    uvec3 lightClusterIndex   = LightClusterIndex(in_NDCPosition);
    uint lightClusterIndex1D  = LightClusterIndexTo1D(lightClusterIndex);
    uint lightCount           = lightClusters[lightClusterIndex1D].count;
    float totalLightIntensity = 0;
    for (uint i = 0; i < lightCount; i++) {
        uint lightIndex        = lightClusters[lightClusterIndex1D].index[i];
        vec3 lightPosition     = lightsBuffer.lights[lightIndex].position;
        float lightRange       = lightsBuffer.lights[lightIndex].range;
        float lightDistance    = distance(lightPosition, in_WorldPosition);
        float lightAttenuation = LightAttenuation(lightDistance, lightRange, 1, 0.5);
        totalLightIntensity += clamp(lightAttenuation, 0, 1);
    }
    fragColor.xyz = vec3(totalLightIntensity);
    fragColor.a   = 1;
}