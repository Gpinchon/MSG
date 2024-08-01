#ifndef VTFS_INPUTS_GLSL
#define VTFS_INPUTS_GLSL
#ifndef __cplusplus
#include <Bindings.glsl>
#include <VTFS.glsl>

layout(std430, binding = SSBO_VTFS_LIGHTS) readonly buffer VTFSLightsBufferSSBO
{
    LightBase lightBase[VTFS_BUFFER_MAX];
};
layout(std430, binding = SSBO_VTFS_LIGHTS) readonly buffer VTFSLightSpotBufferSSBO
{
    LightSpot lightSpot[VTFS_BUFFER_MAX];
};
layout(std430, binding = SSBO_VTFS_LIGHTS) readonly buffer VTFSLightDirBufferSSBO
{
    LightDirectional lightDirectional[VTFS_BUFFER_MAX];
};
layout(std430, binding = SSBO_VTFS_LIGHTS) readonly buffer VTFSLightIBLBufferSSBO
{
    LightIBL lightIBL[VTFS_BUFFER_MAX];
};

layout(std430, binding = SSBO_VTFS_CLUSTERS) readonly buffer VTFSClustersSSBO
{
    VTFSCluster vtfsClusters[VTFS_CLUSTER_COUNT];
};

layout(binding = SAMPLERS_VTFS_IBL) uniform samplerCube u_IBLSamplers[SAMPLERS_VTFS_IBL_COUNT];
#endif //__cplusplus
#endif // VTFS_INPUTS_GLSL