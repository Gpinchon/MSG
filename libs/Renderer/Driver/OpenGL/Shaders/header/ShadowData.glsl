#ifndef SHADOW_DATA_GLSL
#define SHADOW_DATA_GLSL

#include <Bindings.glsl>
#include <Lights.glsl>

#ifdef __cplusplus
namespace Msg::Renderer::GLSL {
#endif //__cplusplus
struct ShadowBase {
    LightBase light;
    uint viewportIndex;
    uint viewportCount;
    float blurRadius;
    float bias;
    float normalBias;
    float minDepth;
    float maxDepth;
    uint _padding[1];
};
struct ShadowPoint {
    LightPoint light;
    uint viewportIndex;
    uint viewportCount;
    float blurRadius;
    float bias;
    float normalBias;
    float minDepth;
    float maxDepth;
    uint _padding[1];
};
struct ShadowSpot {
    LightSpot light;
    uint viewportIndex;
    uint viewportCount;
    float blurRadius;
    float bias;
    float normalBias;
    float minDepth;
    float maxDepth;
    uint _padding[1];
};
struct ShadowDir {
    LightDirectional light;
    uint viewportIndex;
    uint viewportCount;
    float blurRadius;
    float bias;
    float normalBias;
    float minDepth;
    float maxDepth;
    uint _padding[1];
};

struct ShadowsBase {
    uint count;
    uint _padding[3];
    ShadowBase shadows[SAMPLERS_SHADOW_COUNT];
};
struct ShadowsPoint {
    uint count;
    uint _padding[3];
    ShadowPoint shadows[SAMPLERS_SHADOW_COUNT];
};
struct ShadowsSpot {
    uint count;
    uint _padding[3];
    ShadowSpot shadows[SAMPLERS_SHADOW_COUNT];
};
struct ShadowsDir {
    uint count;
    uint _padding[3];
    ShadowDir shadows[SAMPLERS_SHADOW_COUNT];
};
#ifdef __cplusplus
static_assert(sizeof(ShadowBase) == sizeof(ShadowPoint));
static_assert(sizeof(ShadowPoint) == sizeof(ShadowSpot));
static_assert(sizeof(ShadowSpot) == sizeof(ShadowDir));
static_assert(sizeof(ShadowBase) % 16 == 0);
}
#endif //__cplusplus

#endif