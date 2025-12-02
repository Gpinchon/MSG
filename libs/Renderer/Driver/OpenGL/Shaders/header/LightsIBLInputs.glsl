#ifndef LIGHTS_IBL_INPUTS
#define LIGHTS_IBL_INPUTS

#include <Bindings.glsl>
#include <Lights.glsl>
#include <SphericalHarmonics.glsl>

#ifdef __cplusplus
namespace Msg::Renderer::GLSL {
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
INLINE vec3 BoxProject(IN(vec3) a_Pos, IN(vec3) a_V, IN(vec3) a_Min, IN(vec3) a_Max)
{
    vec3 firstPlane    = (a_Max - a_Pos) / a_V;
    vec3 secondPlane   = (a_Min - a_Pos) / a_V;
    vec3 furthestPlane = max(firstPlane, secondPlane);
    float dist         = min(min(furthestPlane.x, furthestPlane.y), furthestPlane.z);
    vec3 intersectPos  = a_Pos + a_V * dist;
    return normalize(intersectPos - (a_Min + a_Max) * 0.5f);
}

vec3 GetIBLColor(IN(BRDF) a_BRDF, IN(vec2) a_BRDFLutSample, IN(vec3) a_WorldPosition, IN(vec3) a_N, IN(vec3) a_V, IN(float) a_NdotV)
{
    const vec3 R         = -reflect(a_V, a_N);
    vec3 totalLightColor = vec3(0);
    for (uint lightIndex = 0; lightIndex < u_LightIBL.count; lightIndex++) {
        const LightIBL light       = u_LightIBL.lights[lightIndex];
        const vec3 lightPosition   = light.commonData.position;
        const bool isInf           = any(isinf(light.halfSize));
        const vec3 lightMin        = lightPosition - light.halfSize;
        const vec3 lightMax        = lightPosition + light.halfSize;
        const vec3 refVec          = (!isInf && light.boxProjection) ? BoxProject(a_WorldPosition, R, lightMin, lightMax) : R;
        const vec3 diffVec         = (!isInf && light.boxProjection) ? BoxProject(a_WorldPosition, a_N, lightMin, lightMax) : a_N;
        const vec3 lightSpecular   = sampleLod(u_LightIBLSamplers[lightIndex], refVec, pow(a_BRDF.alpha, 1.f / 2.f)).rgb;
        const vec3 lightColor      = light.commonData.color;
        const float lightIntensity = light.commonData.intensity;
        if (!isInf) {
            if (any(lessThan(a_WorldPosition, lightMin)) || any(greaterThan(a_WorldPosition, lightMax)))
                continue;
        }
        const vec3 F        = F_Schlick(a_BRDF, a_NdotV);
        const vec3 diffuse  = a_BRDF.cDiff * SampleSH(light.irradianceCoefficients, diffVec);
        const vec3 specular = lightSpecular * (F * a_BRDFLutSample.x + a_BRDFLutSample.y);
        totalLightColor += (diffuse + specular) * lightColor * lightIntensity;
    }
    return totalLightColor;
}
#endif BRDF_GLSL
#endif //__cplusplus
#endif // LIGHTS_IBL_INPUTS