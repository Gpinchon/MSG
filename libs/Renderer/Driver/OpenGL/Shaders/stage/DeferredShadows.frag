#include <Bindings.glsl>
#include <Camera.glsl>
#include <DeferredGBufferData.glsl>
#include <Fog.glsl>
#include <FrameInfo.glsl>
#include <LightsShadowInputs.glsl>

//////////////////////////////////////// STAGE INPUTS
layout(location = 0) in vec2 in_UV;
//////////////////////////////////////// STAGE INPUTS

//////////////////////////////////////// STAGE OUTPUTS
layout(location = 0) out vec4 out_Final;
//////////////////////////////////////// STAGE OUTPUTS

//////////////////////////////////////// UNIFORMS
layout(binding = UBO_FRAME_INFO) uniform FrameInfoBlock
{
    FrameInfo u_FrameInfo;
};
layout(binding = UBO_CAMERA) uniform CameraBlock
{
    Camera u_Camera;
};
layout(binding = UBO_FOG_CAMERA) uniform FogCameraBlock
{
    Camera u_FogCamera;
};
layout(binding = UBO_FOG_SETTINGS) uniform FogSettingsBlock
{
    FogSettings u_FogSettings;
};
layout(binding = SAMPLERS_FOG) uniform sampler3D u_FogScatteringTransmittance;
layout(binding = 0, rgba32ui) restrict uniform uimage2D img_GBuffer0;
layout(binding = 1, rgba32ui) restrict uniform uimage2D img_GBuffer1;
//////////////////////////////////////// UNIFORMS

vec3 GetLightColor(
    IN(BRDF) a_BRDF,
    IN(vec3) a_WorldPosition,
    IN(vec3) a_Normal,
    IN(float) a_FogTransmittance)
{
    const vec3 V = normalize(u_Camera.position - a_WorldPosition);
    vec3 N       = gl_FrontFacing ? a_Normal : -a_Normal;
    float NdotV  = dot(N, V);
    return GetShadowLightColor(a_BRDF, a_WorldPosition, (1 - a_FogTransmittance), N, V, gl_FragCoord.xy, u_FrameInfo.frameIndex);
}

void main()
{
    ivec2 texCoord = ivec2(gl_FragCoord.xy);
    GBufferDataPacked gbufferDataPacked;
    gbufferDataPacked.data0 = imageLoad(img_GBuffer0, texCoord);
    gbufferDataPacked.data1 = imageLoad(img_GBuffer1, texCoord);
    GBufferData gBufferData = UnpackGBufferData(gbufferDataPacked);

    const mat4x4 VP      = u_Camera.projection * u_Camera.view;
    const mat4x4 invVP   = inverse(VP);
    const vec3 NDCPos    = vec3(in_UV * 2.f - 1.f, gBufferData.ndcDepth);
    const vec4 projPos   = invVP * vec4(NDCPos, 1);
    const vec3 worldPos  = projPos.xyz / projPos.w;
    const vec3 worldNorm = gBufferData.normal;

    const mat4x4 fogVP                    = u_FogCamera.projection * u_FogCamera.view;
    const vec4 fogProjPos                 = fogVP * vec4(worldPos, 1);
    const vec3 fogNDC                     = fogProjPos.xyz / fogProjPos.w;
    const vec3 fogTextureSize             = textureSize(u_FogScatteringTransmittance, 0);
    const vec3 fogUVW                     = FogUVWFromNDC(fogNDC, u_FogSettings.depthExponant);
    const vec4 fogScatteringTransmittance = texture(u_FogScatteringTransmittance, fogUVW);

    const vec3 lightColor = GetLightColor(
        gBufferData.brdf,
        worldPos,
        worldNorm,
        fogScatteringTransmittance.a);
    out_Final = vec4(lightColor, 1);
}