#ifndef LIGHTS_GLSL
#define LIGHTS_GLSL

#include <Functions.glsl>

#define SQR(x)              pow(x, 2)
#define LIGHT_CLUSTER_X     16
#define LIGHT_CLUSTER_Y     16
#define LIGHT_CLUSTER_Z     16
#define LIGHT_CLUSTER_COUNT (LIGHT_CLUSTER_X * LIGHT_CLUSTER_Y * LIGHT_CLUSTER_Z)
// The clusters depth subdivision exponent, set to 1 for linear
#define LIGHT_CLUSTER_DEPTH_EXP (1 / 2.f)
// Max nbr of lights per cluster
#define LIGHT_CLUSTER_MAX 128
// Max nbr of lights in light buffer
#define LIGHT_BUFFER_MAX 1024

#ifdef __cplusplus
namespace TabGraph::Renderer::GLSL {
#endif //__cplusplus

struct LightBase {
    vec3 position;
    float range;
    int _padding[8];
};

struct LightPoint {
    vec3 position;
    float range;
    int _padding[8];
};

struct LightSpot {
    vec3 position;
    float range;
    vec3 direction;
    int _padding0[1];
    float innerConeAngle;
    float outerConeAngle;
    int _padding1[2];
};

struct LightDirectional {
    vec3 position;
    float range;
    vec3 direction;
    uint _padding0[1];
    vec3 halfSize;
    int _padding1[1];
};

struct LightClusterAABB {
    vec3 minPoint;
    uint _padding0[1];
    vec3 maxPoint;
    uint _padding1[1];
};

struct LightCluster {
    LightClusterAABB aabb;
    uint count;
    uint _padding[3];
    uint index[LIGHT_CLUSTER_MAX];
};

struct LightsBuffer {
    uint count;
    uint _padding[3];
    LightBase lights[LIGHT_BUFFER_MAX];
};

INLINE float LightAttenuation(float a_Distance, float a_Radius,
    float a_MaxIntensity, float a_Falloff)
{
    float s = a_Distance / a_Radius;
    if (s >= 1.0)
        return 0.0;
    float s2 = pow(s, 2.f);
    return a_MaxIntensity * pow(1 - s2, 2.f) / (1 + a_Falloff * s);
}

/*
 * @return the cluster index corresponding to the NDC position,
 * used during rendering to get the light cluster from NDC
 */
INLINE uvec3 LightClusterIndex(IN(vec3) a_NDCPosition)
{
    vec3 clusterPosition = a_NDCPosition * 0.5f + 0.5f;
    clusterPosition.z    = pow(clusterPosition.z, 1.f / LIGHT_CLUSTER_DEPTH_EXP);
    return uvec3(clusterPosition * vec3(LIGHT_CLUSTER_X, LIGHT_CLUSTER_Y, LIGHT_CLUSTER_Z));
}

INLINE uint LightClusterIndexTo1D(IN(uvec3) a_LightClusterIndex)
{
    return (a_LightClusterIndex.z * LIGHT_CLUSTER_X * LIGHT_CLUSTER_Y) + (a_LightClusterIndex.y * LIGHT_CLUSTER_X) + a_LightClusterIndex.x;
}

INLINE LightBase CreateNDCLight(IN(LightBase) a_WorldLight, IN(mat4x4) a_MVP)
{
    vec3 worldLightLimit = vec3(
        a_WorldLight.position.x + a_WorldLight.range,
        a_WorldLight.position.y,
        a_WorldLight.position.z);
    vec4 viewLightPos   = a_MVP * vec4(a_WorldLight.position, 1);
    vec4 viewLightLimit = a_MVP * vec4(worldLightLimit, 1);
    vec3 NDCLightPos    = vec3(viewLightPos) / viewLightPos.w;
    vec3 NDCLightLimit  = vec3(viewLightLimit) / viewLightLimit.w;
    float NDCLightRange = distance(NDCLightPos, NDCLightLimit);
    LightBase NDCLight;
    NDCLight.position = NDCLightPos;
    NDCLight.range    = NDCLightRange;
    return NDCLight;
}

INLINE bool LightIntersectsAABB(
    IN(LightBase) a_Light,
    IN(LightClusterAABB) a_AABB)
{
    // closest point on the AABB to the sphere center
    vec3 closestPoint = clamp(a_Light.position, a_AABB.minPoint, a_AABB.maxPoint);
    vec3 diff         = closestPoint - a_Light.position;
    // squared distance between the sphere center and closest point
    float distanceSquared = dot(diff, diff);
    return distanceSquared <= a_Light.range;
}

#ifdef __cplusplus
// Ensure every light type have the same size to allow UBO "casting"
static_assert(sizeof(LightBase) == sizeof(LightPoint));
static_assert(sizeof(LightBase) == sizeof(LightSpot));
static_assert(sizeof(LightBase) == sizeof(LightDirectional));
static_assert(sizeof(LightBase) % 16 == 0);
static_assert(sizeof(LightClusterAABB) % 16 == 0);
static_assert(sizeof(LightCluster) % 16 == 0);
static_assert(sizeof(LightsBuffer) % 16 == 0);
}
#endif //__cplusplus

#endif // LIGHTS_GLSL