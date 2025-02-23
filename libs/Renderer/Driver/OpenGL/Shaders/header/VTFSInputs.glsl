#ifndef VTFS_INPUTS_GLSL
#define VTFS_INPUTS_GLSL
#ifndef __cplusplus
#include <Bindings.glsl>
#include <VTFS.glsl>

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
#endif //__cplusplus
#endif // VTFS_INPUTS_GLSL