#ifndef FWD_LIGHTS_GLSL
#define FWD_LIGHTS_GLSL

#include <Bindings.glsl>
#include <Lights.glsl>

#define FWD_LIGHT_MAX_IBL     5 /** max number of IBL lights for each forward rendered primitive */
#define FWD_LIGHT_MAX_SHADOWS 5 /** max number of shadow casting lights for each forward rendered primitive */

#ifdef __cplusplus
namespace MSG::Renderer::GLSL {
struct FwdIBL {
    uint count;
    uint _padding[3];
    LightIBL lights[FWD_LIGHT_MAX_IBL];
};
}
#else
layout(binding = UBO_FWD_IBL) uniform FwdIBLBlock
{
    uint u_FwdIBLCount;
    LightIBL u_FwdIBLLights[FWD_LIGHT_MAX_IBL];
};
layout(binding = SAMPLERS_FWD_IBL) uniform samplerCube u_FwdIBLSamplers[SAMPLERS_FWD_IBL_COUNT];
#endif //__cplusplus

#endif // FWD_LIGHTS_GLSL