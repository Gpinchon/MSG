#ifndef VTFS_GLSL
#define VTFS_GLSL

#include <Lights.glsl>

#define VTFS_CLUSTER_X     32
#define VTFS_CLUSTER_Y     32
#define VTFS_CLUSTER_Z     32
#define VTFS_CLUSTER_COUNT (VTFS_CLUSTER_X * VTFS_CLUSTER_Y * VTFS_CLUSTER_Z)
#define VTFS_LOCAL_SIZE    64
// The clusters depth subdivision exponent, set to 1 for linear
#define VTFS_CLUSTER_DEPTH_EXP (1 / 4.f)
// Max nbr of lights per cluster
#define VTFS_CLUSTER_MAX 128
// Max nbr of lights in light buffer
#define VTFS_BUFFER_MAX 1024
// Max nbr of IBL lights
#define VTFS_IBL_MAX 5

#ifdef __cplusplus
#include <vector>
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
 * @brief transforms the NDC position to the Light clusters "position",
 * used to generate the Light clusters
 */
INLINE vec3 VTFSClusterPosition(IN(vec3) a_NDCPosition)
{
    return vec3(
        a_NDCPosition.x, a_NDCPosition.y,
        pow(a_NDCPosition.z * 0.5f + 0.5f, VTFS_CLUSTER_DEPTH_EXP) * 2.f - 1.f);
}

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
    vec3 worldLightLimit = a_Position + normalize(vec3(1)) * a_Radius;
    vec4 viewLightPos    = a_MVP * vec4(a_Position, 1);
    vec4 viewLightLimit  = a_MVP * vec4(worldLightLimit, 1);
    vec3 NDCLightLimit   = vec3(viewLightLimit) / viewLightLimit.w;
    a_Position           = vec3(viewLightPos) / viewLightPos.w;
    a_Radius             = distance(a_Position, NDCLightLimit) * 2;
}

INLINE void ProjectConeToNDC(
    INOUT(vec3) a_Position, INOUT(vec3) a_Direction, INOUT(float) a_Radius,
    IN(mat4x4) a_MVP)
{
    ProjectSphereToNDC(a_Position, a_Radius, a_MVP);
    a_Direction = vec3(normalize(a_MVP * vec4(a_Direction, 0)));
}

INLINE void ProjectAABBToNDC(
    INOUT(vec3) a_Min, INOUT(vec3) a_Max,
    IN(mat4x4) a_MVP)
{
    vec4 minPos = a_MVP * vec4(a_Min, 1);
    vec4 maxPos = a_MVP * vec4(a_Max, 1);
    a_Min       = vec3(minPos) / minPos.w;
    a_Max       = vec3(maxPos) / maxPos.w;
}

// taken from https://en.wikipedia.org/wiki/Bounding_volume#Basic_intersection_checks
INLINE bool AabbIntersectsAabb(
    IN(vec3) a_AabbMin0, IN(vec3) a_AabbMax0,
    IN(vec3) a_AabbMin1, IN(vec3) a_AabbMax1)
{
    bool res = a_AabbMin0.x > a_AabbMax1.x || a_AabbMin1.x > a_AabbMax0.x
        || a_AabbMin0.y > a_AabbMax1.y || a_AabbMin1.y > a_AabbMax0.y
        || a_AabbMin0.z > a_AabbMax1.z || a_AabbMin1.z > a_AabbMax0.z;
    return !res;
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
    return distanceSquared <= a_SphereRadius * a_SphereRadius;
}

INLINE bool ConeIntersectsAABB(
    IN(vec3) a_ConePosition, IN(vec3) a_ConeDirection, IN(float) a_ConeAngle, IN(float) a_ConeRadius,
    IN(vec3) a_AABBMin, IN(vec3) a_AABBMax)
{
    // closest point on the AABB to the sphere center
    vec3 closestPoint = clamp(a_ConePosition, a_AABBMin, a_AABBMax);
    vec3 diff         = closestPoint - a_ConePosition;
    // squared distance between the sphere center and closest point
    float distanceSquared = dot(diff, diff);
    if (!(distanceSquared <= a_ConeRadius * a_ConeRadius))
        return false;
    // if outside the AABB
    if (closestPoint != a_ConePosition) {
        vec3 closestDir    = normalize(a_ConePosition - closestPoint);
        float closestAngle = dot(closestDir, a_ConeDirection);
        return closestAngle <= a_ConeAngle;
    }
    return true;
}

#ifdef __cplusplus
/*
 * @brief VTFS clusters bounding box are generated only once and never change so they're only generated on the CPU
 */
INLINE std::vector<GLSL::VTFSCluster> GenerateVTFSClusters()
{
    constexpr glm::vec3 clusterSize = {
        1.f / VTFS_CLUSTER_X,
        1.f / VTFS_CLUSTER_Y,
        1.f / VTFS_CLUSTER_Z,
    };
    std::vector<GLSL::VTFSCluster> clusters(VTFS_CLUSTER_COUNT);
    for (uint z = 0; z < VTFS_CLUSTER_Z; ++z) {
        for (uint y = 0; y < VTFS_CLUSTER_Y; ++y) {
            for (uint x = 0; x < VTFS_CLUSTER_X; ++x) {
                glm::vec3 NDCMin           = (glm::vec3(x, y, z) * clusterSize) * 2.f - 1.f;
                glm::vec3 NDCMax           = NDCMin + clusterSize * 2.f;
                auto lightClusterIndex     = GLSL::VTFSClusterIndexTo1D({ x, y, z });
                auto& lightCluster         = clusters[lightClusterIndex];
                lightCluster.aabb.minPoint = GLSL::VTFSClusterPosition(NDCMin);
                lightCluster.aabb.maxPoint = GLSL::VTFSClusterPosition(NDCMax);
            }
        }
    }
    return clusters;
}

static_assert(VTFS_CLUSTER_COUNT % VTFS_LOCAL_SIZE == 0);
static_assert(sizeof(VTFSClusterAABB) % 16 == 0);
static_assert(sizeof(VTFSCluster) % 16 == 0);
static_assert(sizeof(VTFSLightsBuffer) % 16 == 0);
}
#endif //__cplusplus

#endif // VTFS_GLSL