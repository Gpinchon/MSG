#ifndef FWD_LIGHTS_GLSL
#define FWD_LIGHTS_GLSL

#include <Bindings.glsl>
#include <Lights.glsl>

#define FWD_LIGHT_MAX_IBL     SAMPLERS_FWD_IBL_COUNT /** max number of IBL lights for each forward rendered primitive */
#define FWD_LIGHT_MAX_SHADOWS SAMPLERS_FWD_SHADOW_COUNT /** max number of shadow casting lights for each forward rendered primitive */

#ifdef __cplusplus
namespace MSG::Renderer::GLSL {
struct FwdIBL {
    uint count;
    uint _padding[3];
    LightIBL lights[FWD_LIGHT_MAX_IBL];
};
struct FwdShadows {
    uint count;
    uint _padding[3];
    LightBase lights[SAMPLERS_FWD_SHADOW_COUNT];
};
}
#else
////////////////////////////////////////////////////////////////////////////////
// Image Based Lights
////////////////////////////////////////////////////////////////////////////////
layout(binding = UBO_FWD_IBL) uniform FwdIBLBlock
{
    uint u_FwdIBLCount;
    LightIBL u_FwdIBLLights[FWD_LIGHT_MAX_IBL];
};
layout(binding = SAMPLERS_FWD_IBL) uniform samplerCube u_FwdIBLSamplers[FWD_LIGHT_MAX_IBL];

////////////////////////////////////////////////////////////////////////////////
// Shadow Casting Lights
////////////////////////////////////////////////////////////////////////////////
layout(std430, binding = UBO_FWD_SHADOWS) readonly buffer VTFSLightsBufferSSBO
{
    uint u_FwdShadowsCount;
    LightBase u_FwdShadowsBase[FWD_LIGHT_MAX_SHADOWS];
};
layout(std430, binding = UBO_FWD_SHADOWS) readonly buffer VTFSLightPointBufferSSBO
{
    uint u_FwdShadowsPointCount;
    LightPoint u_FwdShadowsPoint[FWD_LIGHT_MAX_SHADOWS];
};
layout(std430, binding = UBO_FWD_SHADOWS) readonly buffer VTFSLightSpotBufferSSBO
{
    uint u_FwdShadowsSpotCount;
    LightSpot u_FwdShadowsSpot[FWD_LIGHT_MAX_SHADOWS];
};
layout(std430, binding = UBO_FWD_SHADOWS) readonly buffer VTFSLightDirBufferSSBO
{
    uint u_FwdShadowsDirCount;
    LightDirectional u_FwdShadowsDir[FWD_LIGHT_MAX_SHADOWS];
};
layout(binding = SAMPLERS_FWD_SHADOW) uniform sampler2DShadow u_FwdShadowSamplers2D[FWD_LIGHT_MAX_SHADOWS];
layout(binding = SAMPLERS_FWD_SHADOW) uniform samplerCubeShadow u_FwdShadowSamplersCube[FWD_LIGHT_MAX_SHADOWS];
layout(binding = SAMPLERS_FWD_SHADOW) uniform sampler2DArrayShadow u_FwdShadowSamplers2DArray[FWD_LIGHT_MAX_SHADOWS];
#endif //__cplusplus

#endif // FWD_LIGHTS_GLSL