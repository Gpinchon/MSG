#ifndef LIGHTS_IBL_INPUTS
#define LIGHTS_IBL_INPUTS

#include <Bindings.glsl>
#include <Lights.glsl>
#include <SphericalHarmonics.glsl>

#ifdef __cplusplus
namespace MSG::Renderer::GLSL {
#endif //__cplusplus
struct LightsIBLUBO {
    uint count;
    uint _padding[3];
    LightIBL lights[SAMPLERS_IBL_COUNT];
};
#ifdef __cplusplus
static_assert(sizeof(LightsIBLUBO) % 16 == 0);
}
#endif //__cplusplus

#ifndef __cplusplus
////////////////////////////////////////////////////////////////////////////////
// Image Based Lights
////////////////////////////////////////////////////////////////////////////////
layout(binding = UBO_FWD_IBL) uniform LightIBLBlock
{
    LightsIBLUBO u_LightIBL;
};
layout(binding = SAMPLERS_IBL) uniform samplerCube u_LightIBLSamplers[SAMPLERS_IBL_COUNT];

#ifdef BRDF_GLSL
vec3 GetIBLColor(IN(BRDF) a_BRDF, IN(vec2) a_BRDFLutSample, IN(vec3) a_WorldPosition, IN(float) a_Occlusion, IN(vec3) a_N, IN(vec3) a_V, IN(float) a_NdotV)
{
    const vec3 R         = -reflect(a_V, a_N);
    vec3 totalLightColor = vec3(0);
    for (uint lightIndex = 0; lightIndex < u_LightIBL.count; lightIndex++) {
        const LightIBL light       = u_LightIBL.lights[lightIndex];
        const vec3 lightSpecular   = sampleLod(u_LightIBLSamplers[lightIndex], R, pow(a_BRDF.alpha, 1.f / 2.f)).rgb;
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
#endif BRDF_GLSL
#endif //__cplusplus
#endif // LIGHTS_IBL_INPUTS