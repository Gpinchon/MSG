#ifndef VTFS_LIGHT_SAMPLING_GLSL
#define VTFS_LIGHT_SAMPLING_GLSL
#ifndef __cplusplus
#include <VTFSInputs.glsl>

// INLINE vec3 SampleLight(
//     IN(vec3) a_WorldPosition,
//     IN(vec3) a_WorldNormal,
//     IN(uint) a_LightIndex)
// {
//     const int lightType        = lightBase[a_LightIndex].commonData.type;
//     const vec3 lightPosition   = lightBase[a_LightIndex].commonData.position;
//     const vec3 lightColor      = lightBase[a_LightIndex].commonData.color;
//     const float lightRange     = lightBase[a_LightIndex].commonData.range;
//     const float lightIntensity = lightBase[a_LightIndex].commonData.intensity;
//     const float lightFalloff   = lightBase[a_LightIndex].commonData.falloff;

//     float lightAttenuation = 0;
//     if (lightType == LIGHT_TYPE_POINT) {
//         const vec3 lightVec       = (lightPosition - a_WorldPosition);
//         const vec3 lightVecNorm   = normalize(lightVec);
//         const float lightDot      = max(dot(a_WorldNormal, lightVecNorm), 0);
//         const float lightDistance = length(lightVec);
//         lightAttenuation          = lightDot * PointLightAttenuation(lightDistance, lightRange, lightIntensity, lightFalloff);
//     } else if (lightType == LIGHT_TYPE_SPOT) {
//         const vec3 lightVec               = (lightPosition - a_WorldPosition);
//         const vec3 lightVecNorm           = normalize(lightVec);
//         const float lightDistance         = length(lightVec);
//         const float lightDot              = max(dot(a_WorldNormal, lightVecNorm), 0);
//         const vec3 lightDir               = lightSpot[a_LightIndex].direction;
//         const float lightInnerConeAngle   = lightSpot[a_LightIndex].innerConeAngle;
//         const float lightOuterConeAngle   = lightSpot[a_LightIndex].outerConeAngle;
//         const float lightSpotAttenuation  = SpotLightAttenuation(lightVecNorm, lightDir, lightInnerConeAngle, lightOuterConeAngle);
//         const float lightPointAttenuation = PointLightAttenuation(lightDistance, lightRange, lightIntensity, lightFalloff);
//         lightAttenuation                  = lightSpotAttenuation * lightPointAttenuation;
//     }
//     return lightColor * lightAttenuation;
// }
#endif //__cplusplus
#endif // VTFS_LIGHT_SAMPLING_GLSL
