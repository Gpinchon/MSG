#ifndef LIGHTS_IBL_INPUTS
#define LIGHTS_IBL_INPUTS

#include <Bindings.glsl>
#include <LightsIBL.glsl>
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
// Transform in local unit parallax cube space (translated, scaled and rotated)
INLINE vec3 ToLightSpaceDir(IN(LightIBL) a_Light, IN(vec3) a_WorldDir)
{
    return (a_Light.toLightSpace * vec4(a_WorldDir, 0)).xyz;
}

// Transform in local unit parallax cube space (translated, scaled and rotated)
INLINE vec3 ToLightSpacePos(IN(LightIBL) a_Light, IN(vec3) a_WorldPos)
{
    vec4 p = (a_Light.toLightSpace * vec4(a_WorldPos, 1));
    return p.xyz / p.w;
}

INLINE bool IsInside(IN(LightIBL) a_Light, IN(vec3) a_LightSpacePos)
{
    const vec3 LightMax = a_Light.halfSize;
    const vec3 LightMin = -a_Light.halfSize;
    return all(greaterThanEqual(LightMax, a_LightSpacePos)) && all(lessThanEqual(LightMin, a_LightSpacePos));
}

/**
 * @ref Local Image-based lighting with Parallax-corrected Cubemap by S. Lagarde & al.
 */
INLINE vec3 BoxProject(IN(LightIBL) a_Light, IN(vec3) a_LightSpacePos, IN(vec3) a_LightSpaceRay)
{
    const vec3 LightMax = a_Light.halfSize;
    const vec3 LightMin = -a_Light.halfSize;

    // Intersection with OBB converted to unit box space
    vec3 FirstPlaneIntersect  = (LightMax - a_LightSpacePos) / a_LightSpaceRay;
    vec3 SecondPlaneIntersect = (LightMin - a_LightSpacePos) / a_LightSpaceRay;
    vec3 FurthestPlane        = max(FirstPlaneIntersect, SecondPlaneIntersect);
    float Distance            = compMin(FurthestPlane);

    // Use Distance in light space to recover intersection
    // There is an error in Lagarde's original paper,
    // using world's space coordinates prevents from the sampling to be rotated
    // vec3 IntersectPositionWS = a_WorldPos + a_Ray * Distance; <-- THIS IS WRONG
    vec3 IntersectPositionLS = a_LightSpacePos + a_LightSpaceRay * Distance;
    // normalize sampling vector by re-scaling it
    return normalize(IntersectPositionLS);
}

float DistanceToIBLBounds(vec3 a_Position, vec3 a_CubeCenter, vec3 a_CubeHalfSize)
{
    vec3 p = a_Position - a_CubeCenter;
    vec3 d = abs(p) - a_CubeHalfSize;
    return min(max(d.x, max(d.y, d.z)), 0.f) + length(max(d, 0.f));
}

vec3 AABBNormalAt(vec3 a_Position, vec3 a_CubeCenter, vec3 a_CubeHalfSize)
{
    vec3 dir       = (a_Position - a_CubeCenter) / a_CubeHalfSize;
    vec3 dirAbs    = abs(dir);
    float is_max_z = step(dirAbs.x, dirAbs.z) * step(dirAbs.y, dirAbs.z);
    float is_max_y = step(dirAbs.x, dirAbs.y) * (1.0 - is_max_z); // Y is largest/equal to X AND Z is NOT the max
    float is_max_x = 1.0 - is_max_z - is_max_y; // X is the default if neither Y nor Z was the max
    vec3 normal    = vec3(
        is_max_x * sign(dir.x),
        is_max_y * sign(dir.y),
        is_max_z * sign(dir.z));
    return normal;
    /*
    // brancheless version of this
    if (dirAbs.z >= dirAbs.x && dirAbs.z >= dirAbs.y)
        return vec3(0, 0, sign(dir.z));
    else if (dirAbs.y >= dirAbs.x)
        return vec3(0, sign(dir.y), 0);
    else
        return vec3(sign(dir.x), 0, 0);
    */
}

INLINE float GetIntensity(IN(LightIBL) a_Light, IN(vec3) a_LightSpacePos)
{
    vec3 BoxInnerOffset = a_Light.innerBoxOffset;
    vec3 BoxInnerRange  = a_Light.innerBoxHalfSize;
    vec3 BoxInnerMin    = BoxInnerOffset - BoxInnerRange;
    vec3 BoxInnerMax    = BoxInnerOffset + BoxInnerRange;
    vec3 BoxOuterOffset = vec3(0, 0, 0);
    vec3 BoxOuterRange  = a_Light.halfSize;
    // Find to which face of the inner bounds we're pointing at
    vec3 FaceNormal = AABBNormalAt(a_LightSpacePos, BoxInnerOffset, BoxInnerRange);
    // Compute inner and outer plane's distance
    float InDist  = compMax(abs(BoxInnerOffset + FaceNormal * BoxInnerRange));
    float OutDist = compMax(abs(BoxOuterOffset + FaceNormal * BoxOuterRange));
    // Find out the difference between the two AABB planes for normalization, avoid dividing by zero
    float InToOutDist = max(abs(OutDist - InDist), EPSILON);
    // Compute current position's distance to inner bounds using SDF
    // clamp to zero when we're inside inner bounds
    float InToPosDist = max(DistanceToIBLBounds(a_LightSpacePos, BoxInnerOffset, BoxInnerRange), 0);
    return 1 - InToPosDist / InToOutDist;
}

vec3 debugColors[SAMPLERS_IBL_COUNT] = {
    vec3(1, 0, 0),
    vec3(0, 1, 0),
    vec3(0, 0, 1),
    vec3(1, 1, 0),
    vec3(1, 0, 1),
};

vec3 GetIBLColor(IN(BRDF) a_BRDF, IN(vec2) a_BRDFLutSample, IN(vec3) a_WorldPosition, IN(vec3) a_N, IN(vec3) a_V, IN(float) a_NdotV)
{
    const vec3 R         = -reflect(a_V, a_N);
    vec3 totalLightColor = vec3(0);
    for (uint lightIndex = 0; lightIndex < u_LightIBL.count; lightIndex++) {
        const LightIBL light = u_LightIBL.lights[lightIndex];
        bool isInf           = any(isinf(light.halfSize));
        vec3 posLS           = ToLightSpacePos(light, a_WorldPosition);
        vec3 refVec          = R;
        vec3 diffVec         = a_N;
        float lightIntensity = light.commonData.intensity;
        if (!isInf) {
            if (!IsInside(light, posLS))
                continue;
            if (light.boxProjection) {
                refVec         = BoxProject(light, posLS, ToLightSpaceDir(light, refVec));
                diffVec        = BoxProject(light, posLS, ToLightSpaceDir(light, diffVec));
                lightIntensity = lightIntensity * GetIntensity(light, posLS);
            }
        }
        vec3 lightSpecular = sampleLod(u_LightIBLSamplers[lightIndex], refVec, pow(a_BRDF.alpha, 1.f / 2.f)).rgb;
        vec3 lightColor    = light.commonData.color;
        vec3 F             = F_Schlick(a_BRDF, a_NdotV);
        vec3 diffuse       = a_BRDF.cDiff * SampleSH(light.irradianceCoefficients, diffVec);
        vec3 specular      = lightSpecular * (F * a_BRDFLutSample.x + a_BRDFLutSample.y);
        totalLightColor += (diffuse + specular) * lightColor * lightIntensity;
        // totalLightColor += debugColors[lightIndex] * lightIntensity;
    }
    return totalLightColor;
}
#endif BRDF_GLSL
#endif //__cplusplus
#endif // LIGHTS_IBL_INPUTS