#ifndef VTFS_INPUTS_GLSL
#define VTFS_INPUTS_GLSL

#define VTFS_LIGHT_BUFFER_SSBO 0
#define VTFS_CLUSTERS_SSBO     1

#ifndef __cplusplus
#include <VTFS.glsl>

layout(std430, binding = VTFS_LIGHT_BUFFER_SSBO) readonly buffer VTFSLightsBufferSSBO
{
    LightBase lightBase[VTFS_BUFFER_MAX];
};
layout(std430, binding = VTFS_LIGHT_BUFFER_SSBO) readonly buffer VTFSLightSpotBufferSSBO
{
    LightSpot lightSpot[VTFS_BUFFER_MAX];
};
layout(std430, binding = VTFS_LIGHT_BUFFER_SSBO) readonly buffer VTFSLightDirBufferSSBO
{
    LightDirectional lightDirectional[VTFS_BUFFER_MAX];
};

layout(std430, binding = VTFS_CLUSTERS_SSBO) readonly buffer VTFSClustersSSBO
{
    VTFSCluster vtfsClusters[];
};
#endif //__cplusplus

#endif // VTFS_INPUTS_GLSL