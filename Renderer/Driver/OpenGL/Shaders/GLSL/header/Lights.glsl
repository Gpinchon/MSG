#ifndef LIGHTS_GLSL
#define LIGHTS_GLSL

#include <Functions.glsl>

#define LIGHT_CLUSTER_X     16
#define LIGHT_CLUSTER_Y     16
#define LIGHT_CLUSTER_Z     16
#define LIGHT_CLUSTER_COUNT (LIGHT_CLUSTER_X * LIGHT_CLUSTER_Y * LIGHT_CLUSTER_Z)
// Max nbr of lights per cluster
#define LIGHT_CLUSTER_MAX 129

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
    float innerConeAngle;
    float outerConeAngle;
    int _padding[3];
};

struct LightDirectional {
    vec3 position;
    float range;
    vec3 direction;
    vec3 halfSize;
    int _padding[2];
};

struct LightClusterAABB {
    vec3 minPoint;
    vec3 maxPoint;
    uint _padding[2];
};

struct LightCluster {
    LightClusterAABB aabb;
    uint count;
    uint index[LIGHT_CLUSTER_MAX];
};

inline uint LightClusterTo1D(uint x, uint y, uint z)
{
    return (z * LIGHT_CLUSTER_X * LIGHT_CLUSTER_Y) + (y * LIGHT_CLUSTER_X) + x;
}

inline LightBase CreateNDCLight(IN(LightBase) a_WorldLight, IN(mat4x4) a_MVP)
{
    vec3 worldLightLimit = vec3(
        a_WorldLight.position.x + a_WorldLight.range,
        a_WorldLight.position.y,
        a_WorldLight.position.z);
    vec4 viewLightPos   = a_MVP * vec4(a_WorldLight.position, 1);
    vec4 viewLightLimit = a_MVP * vec4(worldLightLimit, 1);
    vec3 NDCLightPos    = vec3(viewLightPos) / viewLightPos.w;
    vec3 NDCLightLimit  = vec3(viewLightLimit) / viewLightLimit.w;
    float NDCLightRange = glm::distance(NDCLightPos, NDCLightLimit);
    return { NDCLightPos, NDCLightRange };
}

inline bool LightIntersectsAABB(IN(LightBase) a_Light, IN(LightClusterAABB) a_AABB)
{
    // closest point on the AABB to the sphere center
    vec3 closestPoint = glm::clamp(a_Light.position, a_AABB.minPoint, a_AABB.maxPoint);
    vec3 diff         = closestPoint - a_Light.position;
    // squared distance between the sphere center and closest point
    float distanceSquared = glm::dot(diff, diff);
    return distanceSquared <= a_Light.range * a_Light.range;
}

#ifdef __cplusplus
// Ensure every light type have the same size to allow UBO "casting"
static_assert(sizeof(LightBase) == sizeof(LightPoint));
static_assert(sizeof(LightBase) == sizeof(LightSpot));
static_assert(sizeof(LightBase) == sizeof(LightDirectional));
static_assert(sizeof(LightBase) % 4 == 0);
static_assert(sizeof(LightCluster) % 4 == 0);
}
#endif //__cplusplus

#endif // LIGHTS_GLSL