#ifndef BRDF_GLSL
#define BRDF_GLSL
//////////////////////////////////////// INCLUDES
#include <Bindings.glsl>
#include <Functions.glsl>
#include <Types.glsl>
//////////////////////////////////////// INCLUDES

#ifdef __cplusplus
namespace MSG::GLSL {
#endif //__cplusplus

struct BRDF {
    vec3 cDiff;
    vec3 f0;
    float alpha;
    float transparency;
};

// shamelessly stolen from https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/main/source/Renderer/shaders/brdf.glsl
vec3 F_Schlick(IN(vec3) a_F0, IN(vec3) a_F90, IN(float) a_Theta)
{
    float x  = clamp(1.0 - a_Theta, 0.0, 1.0);
    float x2 = x * x;
    float x5 = x * x2 * x2;
    return a_F0 + (a_F90 - a_F0) * x5;
}

vec3 F_Schlick(IN(vec3) a_F0, IN(float) a_F90, IN(float) a_Theta)
{
    float x  = clamp(1.0 - a_Theta, 0.0, 1.0);
    float x2 = x * x;
    float x5 = x * x2 * x2;
    return a_F0 + (a_F90 - a_F0) * x5;
}

vec3 F_Schlick(IN(vec3) a_F0, IN(float) a_Theta)
{
    const float f90 = 1.0;
    return F_Schlick(a_F0, f90, a_Theta);
}

vec3 F_SchlickRoughness(IN(vec3) a_F0, IN(float) a_Theta, IN(float) a_Roughness)
{
    const vec3 f90 = max(vec3(1.0 - a_Roughness), a_F0);
    return F_Schlick(a_F0, f90, a_Theta);
}

float V_GGX(IN(float) NdotL, IN(float) NdotV, IN(float) a_Alpha)
{
    float alphaRoughnessSq = a_Alpha * a_Alpha;

    float GGXV = NdotL * sqrt(NdotV * NdotV * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);
    float GGXL = NdotV * sqrt(NdotL * NdotL * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);

    float GGX = GGXV + GGXL;
    if (GGX > 0.0) {
        return 0.5 / GGX;
    }
    return 0.0;
}

float D_GGX(IN(float) NdotH, IN(float) a_Alpha)
{
    float alphaRoughnessSq = a_Alpha * a_Alpha;
    float f                = (NdotH * NdotH) * (alphaRoughnessSq - 1.0) + 1.0;
    return alphaRoughnessSq / (M_PI * f * f);
}

vec3 BRDF_specularGGX(IN(BRDF) a_BRDF, IN(float) VdotH, IN(float) NdotL, IN(float) NdotV, IN(float) NdotH)
{
    const vec3 F               = F_Schlick(a_BRDF.f0, VdotH);
    const float Vis            = V_GGX(NdotL, NdotV, a_BRDF.alpha);
    const float D              = D_GGX(NdotH, a_BRDF.alpha);
    const float specularWeight = 1;

    return specularWeight * F * Vis * D;
}

vec3 GGXSpecular(IN(BRDF) a_BRDF, IN(vec3) a_N, IN(vec3) a_V, IN(vec3) a_L)
{
    const vec3 H      = normalize(a_L - a_V);
    const float NdotH = saturate(dot(a_N, H));
    const float NdotL = saturate(dot(a_N, a_L));
    const float VdotH = saturate(dot(a_V, H));
    const float NdotV = saturate(dot(a_N, a_V));

    return BRDF_specularGGX(a_BRDF, VdotH, NdotL, NdotV, NdotH);
}

#ifdef __cplusplus
}
#else
layout(binding = SAMPLERS_BRDF_LUT) uniform sampler2D u_BRDFLut;
#endif //__cplusplus
#endif // BRDF_GLSL