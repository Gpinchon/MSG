#ifndef BRDF_GLSL
#define BRDF_GLSL
//////////////////////////////////////// INCLUDES
#include <Functions.glsl>
#include <Types.glsl>
//////////////////////////////////////// INCLUDES

#ifdef __cplusplus
namespace Msg::GLSL {
#endif //__cplusplus
struct BRDF {
    vec3 cDiff;
    vec3 f0;
    float alpha;
    float transparency;
};

// shamelessly stolen from https://github.com/KhronosGroup/glTF-Sample-Renderer/blob/742db42ce90bf36e239c6fde6ffbe4f267fa83a0/source/Renderer/shaders/brdf.glsl
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

vec3 GGXSpecular(IN(BRDF) a_BRDF, IN(vec3) a_N, IN(vec3) a_V, IN(vec3) a_L)
{
    vec3 H      = normalize(a_L + a_V);
    float dotLH = saturate(dot(a_L, H));
    float dotNH = saturate(dot(a_N, H));
    float dotNL = saturate(dot(a_N, a_L));
    float dotNV = saturate(dot(a_N, a_V));

    vec3 F  = F_Schlick(a_BRDF.f0, dotLH);
    float D = D_GGX(dotNH, a_BRDF.alpha);
    float V = V_GGX(dotNL, dotNV, a_BRDF.alpha);

    return F * saturate(dotNL * D * V);
}
#ifdef __cplusplus
}
#endif //__cplusplus
#endif // BRDF_GLSL