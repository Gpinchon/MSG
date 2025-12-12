#ifndef LIGHTS_IBL_GLSL
#define LIGHTS_IBL_GLSL

#include <Lights.glsl>

#ifdef __cplusplus
namespace Msg::Renderer::GLSL {
#endif //__cplusplus
struct LightIBL {
    LightCommon commonData;
    vec3 innerBoxOffset;
    uint padding0[1];
    vec3 innerBoxHalfSize;
    uint padding1[1];
    vec3 halfSize;
    bool boxProjection;
    vec4 irradianceCoefficients[16];
    mat4x4 toLightSpace;
};
#ifdef __cplusplus
static_assert(sizeof(LightIBL) % 16 == 0);
}
#endif //__cplusplus
#endif // LIGHTS_IBL_GLSL