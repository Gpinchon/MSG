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

vec3 F_Schlick(IN(BRDF) a_BRDF, IN(float) a_Theta)
{
    const vec3 f90 = max(vec3(1.0 - a_BRDF.alpha), a_BRDF.f0);
    return F_Schlick(a_BRDF.f0, f90, a_Theta);
}

float V_GGX(IN(float) a_NoL, IN(float) a_NoV, IN(float) a_Alpha)
{
    float alphaRoughnessSq = a_Alpha * a_Alpha;

    float GGXV = a_NoL * sqrt(a_NoV * a_NoV * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);
    float GGXL = a_NoV * sqrt(a_NoL * a_NoL * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);

    float GGX = GGXV + GGXL;
    if (GGX > 0.0) {
        return 0.5 / GGX;
    }
    return 0.0;
}

float D_GGX(IN(float) a_NoH, IN(float) a_Alpha)
{
    float alphaRoughnessSq = a_Alpha * a_Alpha;
    float f                = (a_NoH * a_NoH) * (alphaRoughnessSq - 1.0) + 1.0;
    return alphaRoughnessSq / (M_PI * f * f);
}

vec3 BRDF_specularGGX(IN(BRDF) a_BRDF, IN(float) VdotH, IN(float) a_NoL, IN(float) a_NoV, IN(float) a_NoH)
{
    const vec3 F               = F_Schlick(a_BRDF.f0, VdotH);
    const float Vis            = V_GGX(a_NoL, a_NoV, a_BRDF.alpha);
    const float D              = D_GGX(a_NoH, a_BRDF.alpha);
    const float specularWeight = 1;

    return specularWeight * F * Vis * D;
}

vec3 GGXSpecular(IN(BRDF) a_BRDF, IN(vec3) a_N, IN(vec3) a_V, IN(vec3) a_L)
{
    const vec3 H      = normalize(a_L - a_V);
    const float a_NoH = saturate(dot(a_N, H));
    const float a_NoL = saturate(dot(a_N, a_L));
    const float VdotH = saturate(dot(a_V, H));
    const float a_NoV = saturate(dot(a_N, a_V));

    return BRDF_specularGGX(a_BRDF, VdotH, a_NoL, a_NoV, a_NoH);
}

#ifdef __cplusplus
}
#else
layout(binding = SAMPLERS_BRDF_LUT) uniform sampler2D u_BRDFLut;

vec2 SampleBRDFLut(IN(BRDF) a_BRDF, IN(float) a_Theta)
{
    return texture(u_BRDFLut, vec2(a_Theta, a_BRDF.alpha)).xy;
}
#endif //__cplusplus
#endif // BRDF_GLSL