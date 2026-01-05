#include <BRDFInputs.glsl>
#include <Bindings.glsl>
#include <Camera.glsl>
#include <DeferredGBufferData.glsl>
#include <FrameInfo.glsl>
#include <LightsVTFSInputs.glsl>

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
layout(binding = 0, rgba32ui) restrict uniform uimage2D img_GBuffer0;
layout(binding = 1, rgba32ui) restrict uniform uimage2D img_GBuffer1;
//////////////////////////////////////// UNIFORMS

vec3 GetLightColor(
    IN(BRDF) a_BRDF,
    IN(vec3) a_NDCPosition,
    IN(vec3) a_WorldPosition,
    IN(vec3) a_Normal)
{
    vec3 totalLightColor = vec3(0);
    const vec3 V         = normalize(u_Camera.position - a_WorldPosition);
    vec3 N               = a_Normal;
    float NdotV          = dot(N, V);
    VTFSSampleParameters params;
    params.brdf                   = a_BRDF;
    params.brdfLutSample          = SampleBRDFLut(a_BRDF, NdotV);
    params.worldPosition          = a_WorldPosition;
    params.worldNormal            = N;
    params.worldView              = V;
    params.normalDotView          = NdotV;
    params.NDCPosition            = a_NDCPosition;
    params.fragCoord              = gl_FragCoord.xy;
    params.frameIndex             = u_FrameInfo.frameIndex;
    params.ignoreIBLs             = true;
    params.ignoreShadowCasters    = true;
    params.ignoreNonShadowCasters = false;
    return GetVTFSLightColor(params);
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

    const vec3 lightColor = GetLightColor(
        gBufferData.brdf,
        NDCPos,
        worldPos,
        worldNorm);
    out_Final = vec4(lightColor, 1);
}