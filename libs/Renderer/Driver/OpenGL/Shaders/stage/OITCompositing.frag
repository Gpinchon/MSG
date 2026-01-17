#include <BRDFInputs.glsl>
#include <Bindings.glsl>
#include <Camera.glsl>
#include <DeferredGBufferData.glsl>
#include <FrameInfo.glsl>
#include <Functions.glsl>
#include <LightsVTFSInputs.glsl>
#include <OIT.glsl>
#include <Random.glsl>

layout(binding = UBO_FRAME_INFO) uniform FrameInfoBlock
{
    FrameInfo u_FrameInfo;
};
layout(binding = UBO_CAMERA) uniform CameraBlock
{
    Camera u_Camera;
};

layout(binding = IMG_OIT_VELOCITY, rg16f) restrict readonly uniform image3D img_Velocity;
layout(binding = IMG_OIT_GBUFFER0, rgba32ui) restrict readonly uniform uimage3D img_GBuffer0;
layout(binding = IMG_OIT_GBUFFER1, rgba32ui) restrict readonly uniform uimage3D img_GBuffer1;
layout(binding = IMG_OIT_DEPTH, r32ui) restrict readonly uniform uimage3D img_Depth;

layout(binding = IMG_OIT_OPAQUE_VELOCITY, rg16f) restrict writeonly uniform image2D img_BackVelocity;
layout(binding = IMG_OIT_OPAQUE_GBUFFER0, rgba32ui) restrict writeonly uniform uimage2D img_BackGBuffer0;
layout(binding = IMG_OIT_OPAQUE_GBUFFER1, rgba32ui) restrict writeonly uniform uimage2D img_BackGBuffer1;

layout(location = 0) in invariant vec2 in_UV;
layout(location = 0) out vec4 out_Color;

vec3 GetLightColor(
    IN(BRDF) a_BRDF,
    IN(vec3) a_NDCPosition,
    IN(vec3) a_WorldPosition,
    IN(vec3) a_Normal)
{
    const vec3 V = normalize(u_Camera.position - a_WorldPosition);
    vec3 N       = a_Normal;
    float NdotV  = dot(N, V);
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
    params.ignoreIBLs             = false;
    params.ignoreShadowCasters    = true;
    params.ignoreNonShadowCasters = true;
    return GetVTFSLightColor(params);
}

vec4 WritePixel(IN(vec2) a_Velocity, IN(uvec4) a_GData0, IN(uvec4) a_GData1)
{
    GBufferDataPacked gbufferDataPacked;
    gbufferDataPacked.data0 = a_GData0;
    gbufferDataPacked.data1 = a_GData1;
    GBufferData gBufferData = UnpackGBufferData(gbufferDataPacked);
    const mat4x4 VP         = u_Camera.projection * u_Camera.view;
    const mat4x4 invVP      = inverse(VP);
    const vec3 NDCPos       = vec3(in_UV * 2.f - 1.f, gBufferData.ndcDepth);
    const vec4 projPos      = invVP * vec4(NDCPos, 1);
    const vec3 worldPos     = projPos.xyz / projPos.w;
    const vec3 worldNorm    = gBufferData.normal;
    const vec3 lightColor   = GetLightColor(
        gBufferData.brdf,
        NDCPos,
        worldPos,
        worldNorm);
    return vec4(
        lightColor + gBufferData.emissive,
        gBufferData.brdf.transparency);
}

void main()
{
    out_Color         = vec4(0);
    bool gDataWritten = false;
    for (uint layer = 0; layer < OIT_LAYERS && out_Color.a < 1; layer++) {
        float ditherVal = Dither(ivec2(gl_FragCoord.xy + layer));
        ivec3 texCoord  = ivec3(gl_FragCoord.xy, layer);
        if (imageLoad(img_Depth, texCoord)[0] == 0xFFFFFFFFu)
            break; // no more occupied layers
        vec4 velocity  = imageLoad(img_Velocity, texCoord);
        uvec4 gBuffer0 = imageLoad(img_GBuffer0, texCoord);
        uvec4 gBuffer1 = imageLoad(img_GBuffer1, texCoord);
        vec4 color     = WritePixel(velocity.xy, gBuffer0, gBuffer1);
        color.rgb *= color.a;
        out_Color.rgb += (1 - out_Color.a) * color.rgb;
        out_Color.a += (1 - out_Color.a) * color.a;
        if ((color.a > ditherVal) && !gDataWritten) {
            imageStore(img_BackVelocity, texCoord.xy, velocity);
            imageStore(img_BackGBuffer0, texCoord.xy, gBuffer0);
            imageStore(img_BackGBuffer1, texCoord.xy, gBuffer1);
            gDataWritten = true;
        }
    }
}