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
namespace Msg::Renderer::GLSL {
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

INLINE float FogLinearizeDepth(IN(float) a_DepthSample, IN(float) a_zNear, IN(float) a_zFar)
{
    float zNDC = a_DepthSample * 2.0f - 1.0f;
    return 2.0f * a_zNear * a_zFar / (a_zFar + a_zNear - zNDC * (a_zFar - a_zNear));
}

INLINE float FogDelinearizeDepth(IN(float) a_LinearDepth, IN(float) a_zNear, IN(float) a_zFar)
{
    float z_n = (a_zFar + a_zNear - 2.0f * a_zNear * a_zFar / max(a_LinearDepth, EPSILON)) / (a_zFar - a_zNear);
    return z_n * 0.5f + 0.5f;
}

INLINE vec3 FogNDCToUVW(IN(vec3) a_NDC, IN(float) a_zNear, IN(float) a_zFar, IN(float) a_DepthExp)
{
    // convert from NDC to texture coordinates and "untweak" voxel's repartion
    float nonLinDepth = pow(a_NDC.z * 0.5 + 0.5, 1.f / a_DepthExp);
    // converts non linear depth to world units
    float linearDepth = FogLinearizeDepth(nonLinDepth, a_zNear, a_zFar);
    // remap linear depth between 0 and 1
    return vec3(
        a_NDC.x * 0.5f + 0.5f,
        a_NDC.y * 0.5f + 0.5f,
        remap(linearDepth, a_zNear, a_zFar, 0.f, 1.f));
}

/**
 * @brief convert's fog cascade's texture coordinates to fog cascade's NDC coordinates
 *
 * @return vec3
 */
INLINE vec3 FogUVWToNDC(IN(vec3) a_UVW, IN(float) a_zNear, IN(float) a_zFar, IN(float) a_DepthExp)
{
    // remap the current texture coordinates to real world units
    float linearDepth = remap(a_UVW.z, 0.f, 1.f, a_zNear, a_zFar);
    // now delinearize it and tweak the voxel's repartion in space
    float nonLinDepth = pow(FogDelinearizeDepth(linearDepth, a_zNear, a_zFar), a_DepthExp);
    return vec3(
        a_UVW.x * 2.0f - 1.0f,
        a_UVW.y * 2.0f - 1.0f,
        nonLinDepth * 2.0f - 1.0f);
}

/** @brief converts world coordinates to fog cascade's NDC coordinates */
INLINE vec3 FogWorldToNDC(IN(vec3) a_WorldPos, IN(mat4x4) a_VP)
{
    vec4 p = a_VP * vec4(a_WorldPos, 1.0f);
    if (p.w > 0.0f)
        return vec3(p.x, p.y, p.z) / p.w;
    else
        return vec3(p.x, p.y, p.z);
}

/** @brief converts fog cascade's NDC coordinates to world coordinates */
INLINE vec3 FogNDCToWorld(IN(vec3) a_NDC, IN(mat4x4) a_InvVP)
{
    vec4 p = a_InvVP * vec4(a_NDC, 1.0f);
    if (p.w > 0)
        return vec3(p.x, p.y, p.z) / p.w;
    else
        return vec3(p.x, p.y, p.z);
}

/** @brief converts world coordinates to cascade's texture coordinates */
INLINE vec3 FogWorldToUVW(IN(vec3) a_WorldPos,
    IN(float) a_ZNear, IN(float) a_ZFar, IN(mat4x4) a_VP, IN(float) a_DepthExp)
{
    vec3 ndc = FogWorldToNDC(a_WorldPos, a_VP);
    return FogNDCToUVW(ndc, a_ZNear, a_ZFar, a_DepthExp);
}

/** @brief converts fog cascade texture coordinates to world coordinates */
INLINE vec3 FogUVWToWorld(IN(vec3) a_UVW, IN(float) a_zNear, IN(float) a_zFar, IN(mat4x4) a_InvVP, IN(float) a_DepthExp)
{
    vec3 ndc = FogUVWToNDC(a_UVW, a_zNear, a_zFar, a_DepthExp);
    return FogNDCToWorld(ndc, a_InvVP);
}

/** @brief converts fog cascade's voxel ID to fog cascade's texture coordinates */
INLINE vec3 FogIDToUVW(IN(ivec3) a_ID, IN(vec3) a_GridSize, IN(float) a_Jitter)
{
    return vec3(
        (float(a_ID.x) + 0.5f) / a_GridSize.x,
        (float(a_ID.y) + 0.5f) / a_GridSize.y,
        max(float(a_ID.z) + 0.5f + a_Jitter, 0.5f) / a_GridSize.z);
}

INLINE vec3 FogIDToUVW(IN(ivec3) a_ID, IN(vec3) a_GridSize)
{
    return FogIDToUVW(a_ID, a_GridSize, 0);
}

/** @brief converts fog voxel ID to fog cascade's NDC coordinates */
INLINE vec3 FogIDToNDC(IN(ivec3) a_ID, IN(float) a_ZNear, IN(float) a_ZFar, IN(vec3) a_GridSize, IN(float) a_DepthExp)
{
    vec3 uv = FogIDToUVW(a_ID, a_GridSize);
    return FogUVWToNDC(uv, a_ZNear, a_ZFar, a_DepthExp);
}

/** @brief converts fog voxel ID to world coordinates */
INLINE vec3 FogIDToWorld(IN(ivec3) a_ID,
    IN(float) a_ZNear, IN(float) a_ZFar, IN(mat4x4) a_InvVP,
    IN(vec3) a_GridSize, IN(float) a_DepthExp)
{
    vec3 uv  = FogIDToUVW(a_ID, a_GridSize);
    vec3 ndc = FogUVWToNDC(uv, a_ZNear, a_ZFar, a_DepthExp);
    return FogNDCToWorld(ndc, a_InvVP);
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