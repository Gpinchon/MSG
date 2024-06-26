#ifndef LIGHTS_GLSL
#define LIGHTS_GLSL

#include <Types.glsl>

#define LIGHT_CLUSTER_X     16
#define LIGHT_CLUSTER_Y     16
#define LIGHT_CLUSTER_Z     16
#define LIGHT_CLUSTER_COUNT (LIGHT_CLUSTER_X * LIGHT_CLUSTER_Y * LIGHT_CLUSTER_Z)
// Max nbr of lights per cluster
#define LIGHT_CLUSTER_MAX 129

#ifdef __cplusplus
namespace TabGraph::Renderer::GLSL {
#endif //__cplusplus

inline uint LightClusterTo1D(uint x, uint y, uint z)
{
    return (z * LIGHT_CLUSTER_X * LIGHT_CLUSTER_Y) + (y * LIGHT_CLUSTER_X) + x;
}

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