#ifndef FWD_LIGHTS_GLSL
#define FWD_LIGHTS_GLSL

#include <Bindings.glsl>
#include <Camera.glsl>
#include <Lights.glsl>
#include <SphericalHarmonics.glsl>

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
    Camera projection;
    LightBase light;
};
struct FwdShadowPoint {
    Camera projection;
    LightPoint light;
};
struct FwdShadowSpot {
    Camera projection;
    LightSpot light;
};
struct FwdShadowDir {
    Camera projection;
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

vec3 GetIBLColor(IN(BRDF) a_BRDF, IN(vec2) a_BRDFLutSample, IN(vec3) a_WorldPosition, IN(float) a_Occlusion, IN(vec3) a_N, IN(vec3) a_V, IN(float) a_NdotV)
{
    const vec3 R         = -reflect(a_V, a_N);
    vec3 totalLightColor = vec3(0);
    for (uint lightIndex = 0; lightIndex < u_FwdIBL.count; lightIndex++) {
        const LightIBL light       = u_FwdIBL.lights[lightIndex];
        const vec3 lightSpecular   = sampleLod(u_FwdIBLSamplers[lightIndex], R, pow(a_BRDF.alpha, 1.f / 2.f)).rgb;
        const vec3 lightPosition   = light.commonData.position;
        const vec3 lightColor      = light.commonData.color;
        const float lightIntensity = light.commonData.intensity;
        const vec3 lightMin        = lightPosition - light.halfSize;
        const vec3 lightMax        = lightPosition + light.halfSize;
        if (any(lessThan(a_WorldPosition, lightMin)) || any(greaterThan(a_WorldPosition, lightMax)))
            continue;
        const vec3 F        = F_Schlick(a_BRDF, a_NdotV);
        const vec3 diffuse  = a_BRDF.cDiff * SampleSH(light.irradianceCoefficients, a_N) * a_Occlusion;
        const vec3 specular = lightSpecular * (F * a_BRDFLutSample.x + a_BRDFLutSample.y);
        totalLightColor += (diffuse + specular) * lightColor * lightIntensity;
    }
    return totalLightColor;
}
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