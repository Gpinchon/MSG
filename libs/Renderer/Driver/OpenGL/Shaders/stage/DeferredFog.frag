#include <Bindings.glsl>
#include <Camera.glsl>
#include <DeferredGBufferData.glsl>
#include <Fog.glsl>
#include <FogCamera.glsl>

//////////////////////////////////////// STAGE INPUTS
layout(location = 0) in vec2 in_UV;
//////////////////////////////////////// STAGE INPUTS

//////////////////////////////////////// STAGE OUTPUTS
layout(location = 0) out vec4 out_Final;
//////////////////////////////////////// STAGE OUTPUTS

//////////////////////////////////////// UNIFORMS
layout(binding = UBO_CAMERA) uniform CameraBlock
{
    Camera u_Camera;
};
layout(binding = UBO_FOG_CAMERA) uniform FogCameraBlock
{
    FogCamera u_FogCamera[FOG_CASCADE_COUNT];
};
layout(binding = UBO_FOG_SETTINGS) uniform FogSettingsBlock
{
    FogSettings u_FogSettings;
};
layout(binding = SAMPLERS_FOG) uniform sampler3D u_FogScatteringTransmittance[FOG_CASCADE_COUNT];
layout(binding = 0, rgba32ui) restrict uniform uimage2D img_GBuffer0;
layout(binding = 1, rgba32ui) restrict uniform uimage2D img_GBuffer1;
//////////////////////////////////////// UNIFORMS

float BeerLaw(IN(float) a_Density, IN(float) a_StepSize)
{
    return exp(-a_Density * a_StepSize);
}

void main()
{
    ivec2 texCoord = ivec2(gl_FragCoord.xy);
    GBufferDataPacked gbufferDataPacked;
    gbufferDataPacked.data0 = imageLoad(img_GBuffer0, texCoord);
    gbufferDataPacked.data1 = imageLoad(img_GBuffer1, texCoord);
    GBufferData gBufferData = UnpackGBufferData(gbufferDataPacked);

    if (gBufferData.ndcDepth == 0)
        gBufferData.ndcDepth = 1;

    const mat4x4 VP      = u_Camera.projection * u_Camera.view;
    const mat4x4 invVP   = inverse(VP);
    const vec3 NDCPos    = vec3(in_UV * 2.f - 1.f, gBufferData.ndcDepth);
    const vec4 projPos   = invVP * vec4(NDCPos, 1);
    const vec3 worldPos  = projPos.xyz / projPos.w;
    const vec3 worldNorm = gBufferData.normal;

    const vec4 camPos   = u_Camera.view * vec4(worldPos, 1);
    const float camDist = -camPos.z;
    uint cascadeI       = 0;
    for (cascadeI = 0; cascadeI < FOG_CASCADE_COUNT; cascadeI++) {
        if (u_FogCamera[cascadeI].current.zNear < camDist && u_FogCamera[cascadeI].current.zFar > camDist)
            break;
    }
    uint currCascadeI = clamp(cascadeI + 0, 0u, FOG_CASCADE_COUNT - 1u);
    // Current cascade
    {
        const mat4x4 fogVP    = u_FogCamera[currCascadeI].current.projection * u_FogCamera[currCascadeI].current.view;
        const vec4 fogProjPos = fogVP * vec4(worldPos, 1);
        const vec3 fogNDC     = fogProjPos.xyz / fogProjPos.w;
        const vec3 fogUVW     = FogUVWFromNDC(fogNDC, u_FogSettings.depthExponant);
        out_Final             = texture(u_FogScatteringTransmittance[currCascadeI], fogUVW);
    }

    // Next cascade
    if (currCascadeI < FOG_CASCADE_COUNT) {
        uint nextCascadeI     = clamp(cascadeI + 1, 0u, FOG_CASCADE_COUNT - 1u);
        const mat4x4 fogVP    = u_FogCamera[nextCascadeI].current.projection * u_FogCamera[nextCascadeI].current.view;
        const vec4 fogProjPos = fogVP * vec4(worldPos, 1);
        const vec3 fogNDC     = fogProjPos.xyz / fogProjPos.w;
        const vec3 fogUVW     = FogUVWFromNDC(fogNDC, u_FogSettings.depthExponant);
        vec4 nextCascade      = texture(u_FogScatteringTransmittance[nextCascadeI], fogUVW);
        float curFar          = u_FogCamera[currCascadeI].current.zFar;
        float nextNear        = u_FogCamera[nextCascadeI].current.zNear;
        float mixValue        = remap(max(camDist, nextNear), nextNear, curFar, 0, 1);
        out_Final             = mix(out_Final, nextCascade, mixValue);
    }

    if (cascadeI == FOG_CASCADE_COUNT) {
        float curDist       = distance(u_Camera.position, worldPos);
        float sliceDist     = curDist - u_FogCamera[cascadeI - 1].current.zFar;
        float transmittance = saturate(BeerLaw(u_FogSettings.globalExtinction, sliceDist));
        out_Final.rgb += u_FogSettings.globalScattering * (1 - transmittance);
        out_Final.a *= transmittance;
    }
}