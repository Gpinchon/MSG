#ifndef VTFS_GLSL
#define VTFS_GLSL

#include <Lights.glsl>

#define VTFS_CLUSTER_X     16
#define VTFS_CLUSTER_Y     16
#define VTFS_CLUSTER_Z     16
#define VTFS_CLUSTER_COUNT (VTFS_CLUSTER_X * VTFS_CLUSTER_Y * VTFS_CLUSTER_Z)
// The clusters depth subdivision exponent, set to 1 for linear
#define VTFS_CLUSTER_DEPTH_EXP (1 / 2.f)
// Max nbr of lights per cluster
#define VTFS_CLUSTER_MAX 128
// Max nbr of lights in light buffer
#define VTFS_BUFFER_MAX 1024

#ifdef __cplusplus
namespace TabGraph::Renderer::GLSL {
#endif //__cplusplus

struct VTFSClusterAABB {
    vec3 minPoint;
    uint _padding0[1];
    vec3 maxPoint;
    uint _padding1[1];
};

struct VTFSCluster {
    VTFSClusterAABB aabb;
    uint count;
    uint _padding[3];
    uint index[VTFS_CLUSTER_MAX];
};

struct VTFSLightsBuffer {
    uint count;
    uint _padding[3];
    LightBase lights[VTFS_BUFFER_MAX];
};

/*
 * @return the cluster index corresponding to the NDC position,
 * used during rendering to get the light cluster from NDC
 */
INLINE uvec3 VTFSClusterIndex(IN(vec3) a_NDCPosition)
{
    vec3 clusterPosition = a_NDCPosition * 0.5f + 0.5f;
    clusterPosition.z    = pow(clusterPosition.z, 1.f / VTFS_CLUSTER_DEPTH_EXP);
    return uvec3(clusterPosition * vec3(VTFS_CLUSTER_X, VTFS_CLUSTER_Y, VTFS_CLUSTER_Z));
}

INLINE uint VTFSClusterIndexTo1D(IN(uvec3) a_LightClusterIndex)
{
    return (a_LightClusterIndex.z * VTFS_CLUSTER_X * VTFS_CLUSTER_Y) + (a_LightClusterIndex.y * VTFS_CLUSTER_X) + a_LightClusterIndex.x;
}

INLINE void ProjectSphereToNDC(
    INOUT(vec3) a_Position, INOUT(float) a_Radius,
    IN(mat4x4) a_MVP)
{
    vec3 worldLightLimit = vec3(
        a_Position.x + a_Radius,
        a_Position.y,
        a_Position.z);
    vec4 viewLightPos   = a_MVP * vec4(a_Position, 1);
    vec4 viewLightLimit = a_MVP * vec4(worldLightLimit, 1);
    vec3 NDCLightLimit  = vec3(viewLightLimit) / viewLightLimit.w;
    a_Position          = vec3(viewLightPos) / viewLightPos.w;
    a_Radius            = distance(a_Position, NDCLightLimit);
}

INLINE bool SphereIntersectsAABB(
    IN(vec3) a_SpherePosition, IN(float) a_SphereRadius,
    IN(vec3) a_AABBMin, IN(vec3) a_AABBMax)
{
    // closest point on the AABB to the sphere center
    vec3 closestPoint = clamp(a_SpherePosition, a_AABBMin, a_AABBMax);
    vec3 diff         = closestPoint - a_SpherePosition;
    // squared distance between the sphere center and closest point
    float distanceSquared = dot(diff, diff);
    return distanceSquared <= a_SphereRadius;
}

#ifdef __cplusplus
static_assert(sizeof(VTFSClusterAABB) % 16 == 0);
static_assert(sizeof(VTFSCluster) % 16 == 0);
static_assert(sizeof(VTFSLightsBuffer) % 16 == 0);
}
#endif //__cplusplus

#endif // VTFS_GLSL