#ifndef LIGHTS_GLSL
#define LIGHTS_GLSL

#include <Functions.glsl>

#define LIGHT_TYPE_UNKNOWN     (-1)
#define LIGHT_TYPE_POINT       (LIGHT_TYPE_UNKNOWN + 1)
#define LIGHT_TYPE_SPOT        (LIGHT_TYPE_POINT + 1)
#define LIGHT_TYPE_DIRECTIONAL (LIGHT_TYPE_SPOT + 1)
#define LIGHT_TYPE_IBL         (LIGHT_TYPE_DIRECTIONAL + 1)

#ifdef __cplusplus
namespace Msg::Renderer::GLSL {
#endif //__cplusplus
struct LightCommon {
    int type;
    float intensity;
    float falloff;
    float radius; // used for culling and early discard
    vec3 position;
    uint priority;
    vec3 color;
    float lightShaftIntensity; // used to make light shafts more visible with fog
};

struct LightBase {
    LightCommon commonData;
    int _padding[8];
};

struct LightPoint {
    LightCommon commonData;
    float range;
    int _padding[7];
};

struct LightSpot {
    LightCommon commonData;
    vec3 direction;
    float range;
    float innerConeAngle;
    float outerConeAngle;
    uint _padding[2];
};

struct LightDirectional {
    LightCommon commonData;
    vec3 direction;
    uint _padding0[1];
    vec3 halfSize;
    uint _padding1[1];
};

INLINE float PointLightIntensity(
    IN(float) a_Distance,
    IN(float) a_Range,
    IN(float) a_MaxIntensity,
    IN(float) a_Falloff)
{
    float distAtt  = 1.f / pow(a_Distance, a_Falloff);
    float rangeAtt = isinf(a_Range) ? 1 : max(min(1.f - float(pow(a_Distance / a_Range, 4.f)), 1.f), 0.f) / a_Distance * a_Distance;
    return a_MaxIntensity * distAtt * rangeAtt;
}

// @brief Taken from https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/9940e4b4f4a2a296351bcd35035cc518deadc298/source/Renderer/shaders/punctual.glsl#L42
INLINE float SpotLightIntensity(
    IN(vec3) a_LightVecNorm, IN(vec3) a_LightDir,
    IN(float) a_InnerConeAngle, IN(float) a_OuterConeAngle)
{
    float actualCos    = dot(a_LightDir, -a_LightVecNorm);
    float outerConeCos = cos(a_OuterConeAngle);
    float innerConeCos = cos(a_InnerConeAngle);
    if (actualCos > outerConeCos) {
        if (actualCos < innerConeCos) {
            float angularAttenuation = (actualCos - outerConeCos) / (innerConeCos - outerConeCos);
            return pow(angularAttenuation, 2.f);
        }
        return 1;
    } else
        return 0;
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