#ifndef LIGHTS_GLSL
#define LIGHTS_GLSL

#include <Functions.glsl>

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

INLINE float LightAttenuation(
    float a_Distance, float a_Radius,
    float a_MaxIntensity, float a_Falloff)
{
    float s = a_Distance / a_Radius;
    if (s >= 1.0)
        return 0.0;
    float s2 = pow(s, 2.f);
    return a_MaxIntensity * pow(1 - s2, 2.f) / (1 + a_Falloff * s);
}

#ifdef __cplusplus
// Ensure every light type have the same size to allow UBO "casting"
static_assert(sizeof(LightBase) == sizeof(LightPoint));
static_assert(sizeof(LightBase) == sizeof(LightSpot));
static_assert(sizeof(LightBase) == sizeof(LightDirectional));
static_assert(sizeof(LightBase) % 16 == 0);
}
#endif //__cplusplus

#endif // LIGHTS_GLSL