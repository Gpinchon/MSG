#ifndef FOG_INPUTS_GLSL
#define FOG_INPUTS_GLSL
#ifndef __cplusplus
#include <Bindings.glsl>
#include <Fog.glsl>
#include <FogCamera.glsl>
#include <Types.glsl>

//////////////////////////////////////// UNIFORMS
layout(binding = UBO_FOG_SETTINGS) uniform FogSettingsBlock
{
    FogSettings u_FogSettings;
};
layout(binding = UBO_FOG_CAMERA) uniform FogCameraBlock
{
    FogCamera u_FogCamera[FOG_CASCADE_COUNT];
};
layout(binding = SAMPLERS_FOG) uniform sampler3D u_FogScatteringTransmittance[FOG_CASCADE_COUNT];
//////////////////////////////////////// UNIFORMS

vec3 FogGetUVW(IN(uint) a_CascadeIndex, IN(vec3) a_WorldPos)
{
    const mat4x4 fogVP    = u_FogCamera[a_CascadeIndex].current.projection * u_FogCamera[a_CascadeIndex].current.view;
    const vec4 fogProjPos = fogVP * vec4(a_WorldPos, 1);
    const vec3 fogNDC     = fogProjPos.xyz / fogProjPos.w;
    return FogUVWFromNDC(fogNDC, u_FogSettings.depthExponant);
}

/**
 * @brief returns the current fog cascade index
 * @arg a_CamDist: the result of -(u_Camera.view * vec4(worldPos, 1)).z
 * @return uint
 */
uint FogGetCascadeIndex(IN(float) a_CamDist)
{
    uint cascadeI = 0;
    for (cascadeI = 0; cascadeI < FOG_CASCADE_COUNT; cascadeI++) {
        if (u_FogCamera[cascadeI].current.zNear < a_CamDist && u_FogCamera[cascadeI].current.zFar > a_CamDist)
            break;
    }
    return min(cascadeI, FOG_CASCADE_COUNT - 1u);
}

/**
 * @brief returns the current fog value at the specified cascade/world position
 * @arg a_CamDist: the result of -(u_Camera.view * vec4(worldPos, 1)).z
 * @return vec4
 */
vec4 FogGetScatteringTransmittance(IN(Camera) a_Camera, IN(uint) a_CascadeIndex, IN(float) a_CamDist, IN(vec3) a_WorldPos)
{
    if (a_CamDist < u_FogCamera[0].current.zNear) { // we must be behind the volumetric fog
        float sliceDist     = distance(a_Camera.position, a_WorldPos);
        float transmittance = saturate(BeerLaw(u_FogSettings.globalExtinction, sliceDist));
        return vec4(u_FogSettings.globalScattering * (1 - transmittance), transmittance);
    }
    vec4 fogScattTrans = texture(u_FogScatteringTransmittance[a_CascadeIndex], FogGetUVW(a_CascadeIndex, a_WorldPos));
    if (a_CascadeIndex < (FOG_CASCADE_COUNT - 1)) {
        const uint nextCascadeI  = a_CascadeIndex + 1;
        const vec4 nextScatTrans = texture(u_FogScatteringTransmittance[nextCascadeI], FogGetUVW(nextCascadeI, a_WorldPos));
        float curFar             = u_FogCamera[a_CascadeIndex].current.zFar;
        float nextNear           = u_FogCamera[nextCascadeI].current.zNear;
        float mixValue           = normalizeValue(max(a_CamDist, nextNear), nextNear, curFar);
        fogScattTrans            = mix(fogScattTrans, nextScatTrans, mixValue);
    } else if (a_CamDist > u_FogCamera[a_CascadeIndex].current.zFar) { // we must be past the volumetric fog
        float curDist       = distance(a_Camera.position, a_WorldPos);
        float sliceDist     = curDist - u_FogCamera[a_CascadeIndex].current.zFar;
        float transmittance = saturate(BeerLaw(u_FogSettings.globalExtinction, sliceDist));
        fogScattTrans.rgb += u_FogSettings.globalScattering * (1 - transmittance);
        fogScattTrans.a *= transmittance;
    }
    return fogScattTrans;
}

vec4 FogGetScatteringTransmittance(IN(Camera) a_Camera, IN(vec3) a_WorldPos)
{
    const vec4 camPos       = a_Camera.view * vec4(a_WorldPos, 1);
    const float camDist     = -camPos.z;
    const uint cascadeIndex = FogGetCascadeIndex(camDist);
    return FogGetScatteringTransmittance(a_Camera, cascadeIndex, camDist, a_WorldPos);
}
#endif //__cplusplus
#endif // FOG_INPUTS_GLSL