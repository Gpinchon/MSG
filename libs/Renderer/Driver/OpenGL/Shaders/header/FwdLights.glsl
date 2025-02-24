#ifndef FWD_LIGHTS_GLSL
#define FWD_LIGHTS_GLSL

#include <Bindings.glsl>
#include <Lights.glsl>

#define FWD_LIGHT_MAX_IBL     SAMPLERS_FWD_IBL_COUNT /** max number of IBL lights for each forward rendered primitive */
#define FWD_LIGHT_MAX_SHADOWS SAMPLERS_FWD_SHADOW_COUNT /** max number of shadow casting lights for each forward rendered primitive */

#ifdef __cplusplus
namespace MSG::Renderer::GLSL {
#endif //__cplusplus
struct FwdIBL {
    uint count;
    uint _padding[3];
    LightIBL lights[FWD_LIGHT_MAX_IBL];
};

struct FwdShadowBase {
    mat4x4 projection;
    LightBase light;
};
struct FwdShadowPoint {
    mat4x4 projection;
    LightPoint light;
};
struct FwdShadowSpot {
    mat4x4 projection;
    LightSpot light;
};
struct FwdShadowDir {
    mat4x4 projection;
    LightDirectional light;
};

struct FwdShadowsBase {
    uint count;
    uint _padding[3];
    FwdShadowBase shadows[FWD_LIGHT_MAX_SHADOWS];
};
struct FwdShadowsPoint {
    uint count;
    uint _padding[3];
    FwdShadowPoint shadows[FWD_LIGHT_MAX_SHADOWS];
};
struct FwdShadowsSpot {
    uint count;
    uint _padding[3];
    FwdShadowSpot shadows[FWD_LIGHT_MAX_SHADOWS];
};
struct FwdShadowsDir {
    uint count;
    uint _padding[3];
    FwdShadowDir shadows[FWD_LIGHT_MAX_SHADOWS];
};
#ifdef __cplusplus
}
#endif //__cplusplus

#ifndef __cplusplus
////////////////////////////////////////////////////////////////////////////////
// Image Based Lights
////////////////////////////////////////////////////////////////////////////////
layout(binding = UBO_FWD_IBL) uniform FwdIBLBlock
{
    FwdIBL u_FwdIBL;
};
layout(binding = SAMPLERS_FWD_IBL) uniform samplerCube u_FwdIBLSamplers[FWD_LIGHT_MAX_IBL];

////////////////////////////////////////////////////////////////////////////////
// Shadow Casting Lights
////////////////////////////////////////////////////////////////////////////////
layout(binding = UBO_FWD_SHADOWS) uniform FwdShadowsBlock
{
    FwdShadowBase u_FwdShadowsBase;
};
layout(binding = UBO_FWD_SHADOWS) uniform FwdShadowsPointBlock
{
    FwdShadowPoint u_FwdShadowsPoint;
};
layout(binding = UBO_FWD_SHADOWS) uniform FwdShadowsSpotBlock
{
    FwdShadowSpot u_FwdShadowsSpot;
};
layout(binding = UBO_FWD_SHADOWS) uniform FwdShadowsDirBlock
{
    FwdShadowDir u_FwdShadowsDir;
};
layout(binding = SAMPLERS_FWD_SHADOW) uniform sampler2DShadow u_FwdShadowSamplers2D[FWD_LIGHT_MAX_SHADOWS];
layout(binding = SAMPLERS_FWD_SHADOW) uniform samplerCubeShadow u_FwdShadowSamplersCube[FWD_LIGHT_MAX_SHADOWS];
layout(binding = SAMPLERS_FWD_SHADOW) uniform sampler2DArrayShadow u_FwdShadowSamplers2DArray[FWD_LIGHT_MAX_SHADOWS];
#endif //__cplusplus

#endif // FWD_LIGHTS_GLSL