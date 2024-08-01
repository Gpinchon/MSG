#ifndef BRDF_GLSL
#define BRDF_GLSL
//////////////////////////////////////// INCLUDES
#include <Functions.glsl>
#include <Types.glsl>
//////////////////////////////////////// INCLUDES

#ifdef __cplusplus
namespace TabGraph::GLSL {
#endif //__cplusplus

struct BRDF {
    vec3 cDiff;
    vec3 f0;
    float alpha;
};

// shamelessly stolen from https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/main/source/Renderer/shaders/brdf.glsl
vec3 F_Schlick(IN(vec3) f0, IN(float) f90, IN(float) VdotH)
{
    float x  = clamp(1.0 - VdotH, 0.0, 1.0);
    float x2 = x * x;
    float x5 = x * x2 * x2;
    return f0 + (f90 - f0) * x5;
}

vec3 F_Schlick(IN(vec3) f0, IN(float) VdotH)
{
    float f90 = 1.0;
    return F_Schlick(f0, f90, VdotH);
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
#endif //__cplusplus
#endif // BRDF_GLSL