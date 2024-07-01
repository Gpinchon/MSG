#include <Lights.glsl>
#include <VTFS.glsl>

layout(local_size_x = VTFS_LOCAL_SIZE, local_size_y = 1, local_size_z = 1) in;

layout(binding = 0) uniform CameraBlock
{
    // TransformUBO transform;
    mat4 projection;
    mat4 view;
}
u_Camera;

layout(std430, binding = 0) readonly buffer VTFSLightsBufferSSBO
{
    VTFSLightsBuffer lights;
};

layout(std430, binding = 1) restrict buffer VTFSClustersSSBO
{
    VTFSCluster clusters[];
};

void main()
{
    mat4x4 MVP              = u_Camera.projection * u_Camera.view;
    const uint clusterIndex = gl_WorkGroupSize.x * gl_WorkGroupID.x + gl_LocalInvocationID.x;
    uint lightCount         = 0;
    for (uint lightIndex = 0; lightIndex < lights.count; ++lightIndex) {
        vec3 lightPosition = lights.lights[lightIndex].position;
        float lightRadius  = lights.lights[lightIndex].range;
        ProjectSphereToNDC(lightPosition, lightRadius, MVP);
        if (SphereIntersectsAABB(
                lightPosition, lightRadius,
                clusters[clusterIndex].aabb.minPoint,
                clusters[clusterIndex].aabb.maxPoint)) {
            clusters[clusterIndex].index[lightCount] = lightIndex;
            lightCount++;
            if (lightCount == VTFS_CLUSTER_MAX)
                break;
        }
    }
    clusters[clusterIndex].count = lightCount;
}
