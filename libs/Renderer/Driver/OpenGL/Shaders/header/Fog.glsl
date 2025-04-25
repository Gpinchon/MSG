#ifndef FOG_GLSL
#define FOG_GLSL

#include <Functions.glsl>

#define FOG_LIGHT_WORKGROUPS_X       16
#define FOG_LIGHT_WORKGROUPS_Y       16
#define FOG_LIGHT_WORKGROUPS_Z       4
#define FOG_INTEGRATION_WORKGROUPS_X 32
#define FOG_INTEGRATION_WORKGROUPS_Y 32

#ifdef __cplusplus
#include <Types.glsl>
namespace MSG::Renderer::GLSL {
#endif //__cplusplus
struct FogSettings {
    vec3 globalScattering;
    float globalExtinction;
    vec3 globalEmissive;
    float globalPhaseG;
    vec3 noiseDensityOffset;
    float noiseDensityScale;
    float noiseDensityIntensity;
    float noiseDensityMaxDist;
    float depthExponant;
    bool fogBackground;
};

INLINE vec3 FogNDCFromUVW(IN(vec3) a_UVW, IN(float) a_Exponant)
{
    return vec3(a_UVW.x, a_UVW.y, pow(a_UVW.z, 1.f / a_Exponant)) * 2.f - 1.f;
}

INLINE vec3 FogUVWFromNDC(IN(vec3) a_NDCPosition, IN(float) a_Exponant)
{
    vec3 uvw = a_NDCPosition * 0.5f + 0.5f;
    uvw.z    = pow(uvw.z, a_Exponant);
    return uvw;
}

INLINE ivec3 FogTexCoordFromNDC(IN(vec3) a_NDCPosition, IN(float) a_Exponant, IN(vec3) a_TexSize)
{
    vec3 uvz = FogUVWFromNDC(a_NDCPosition, a_Exponant);
    return ivec3(uvz * a_TexSize);
}

INLINE float BeerLaw(IN(float) a_Density, IN(float) a_StepSize)
{
    return exp(-a_Density * a_StepSize);
}

#ifdef __cplusplus
static_assert(sizeof(FogSettings) % 16 == 0);
}
#endif //__cplusplus
#endif // FOG_GLSL