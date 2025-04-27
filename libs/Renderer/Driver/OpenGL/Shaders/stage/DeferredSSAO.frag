#include <Bindings.glsl>
#include <Camera.glsl>
#include <DeferredGBufferData.glsl>
#include <FrameInfo.glsl>
#include <Random.glsl>
#include <SSAO.glsl>

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
layout(binding = UBO_CAMERA + 1) uniform SSAOSettingsBlock
{
    SSAOSettings u_SSAOSettings;
};
layout(binding = 0, rgba32ui) restrict uniform uimage2D img_GBuffer0;
layout(binding = 1, rgba32ui) restrict uniform uimage2D img_GBuffer1;
//////////////////////////////////////// UNIFORMS

#if SSAO_QUALITY == 1
#define SAMPLENBR 4
#elif SSAO_QUALITY == 2
#define SAMPLENBR 8
#elif SSAO_QUALITY == 3
#define SAMPLENBR 16
#else // SSAO_QUALITY == 4
#define SAMPLENBR 32
#endif

vec3 GetWorldPosition(IN(vec2) a_UV)
{
    GBufferDataPacked gbufferDataPacked;
    gbufferDataPacked.data0 = imageLoad(img_GBuffer0, ivec2(a_UV * imageSize(img_GBuffer0)));
    gbufferDataPacked.data1 = imageLoad(img_GBuffer1, ivec2(a_UV * imageSize(img_GBuffer1)));
    GBufferData gBufferData = UnpackGBufferData(gbufferDataPacked);
    const mat4x4 VP         = u_Camera.projection * u_Camera.view;
    const mat4x4 invVP      = inverse(VP);
    const vec3 NDCPos       = vec3(a_UV * 2.f - 1.f, gBufferData.ndcDepth);
    const vec4 projPos      = invVP * vec4(NDCPos, 1);
    return projPos.xyz / projPos.w;
}

void main()
{
    ivec2 texCoord = ivec2(gl_FragCoord.xy);
    GBufferDataPacked gbufferDataPacked;
    gbufferDataPacked.data0 = imageLoad(img_GBuffer0, texCoord);
    gbufferDataPacked.data1 = imageLoad(img_GBuffer1, texCoord);
    GBufferData gBufferData = UnpackGBufferData(gbufferDataPacked);

    const mat4x4 VP     = u_Camera.projection * u_Camera.view;
    const mat4x4 invVP  = inverse(VP);
    const vec3 NDCPos   = vec3(in_UV * 2.f - 1.f, gBufferData.ndcDepth);
    const vec4 projPos  = invVP * vec4(NDCPos, 1);
    const vec3 worldPos = projPos.xyz / projPos.w;

    float occlusion   = 0.f;
    const vec3 N      = gBufferData.normal;
    const vec3 P      = worldPos;
    const float noise = InterleavedGradientNoise(gl_FragCoord.xy, u_FrameInfo.frameIndex) * 2 - 1;
    for (int i = 0; i < SAMPLENBR; ++i) {
        vec2 E = Hammersley(i, SAMPLENBR, texCoord) * vec2(M_PI, 2 * M_PI);
        E.y += noise;
        vec2 sE           = vec2(cos(E.y), sin(E.y)) * u_SSAOSettings.radius * cos(E.x);
        vec2 screenCoords = in_UV + sE;
        if (any(lessThan(screenCoords, vec2(0))) || any(greaterThan(screenCoords, vec2(1))))
            continue;
        vec3 V  = GetWorldPosition(screenCoords) - P;
        float d = length(V);
        V /= d;
        d *= u_SSAOSettings.radius;
        occlusion += max(0.0, dot(N, V) - 0.025) * (1.0 / (1.0 + d));
    }
    occlusion /= float(SAMPLENBR);
    occlusion *= u_SSAOSettings.strength;
    occlusion = gBufferData.AO * (1 - occlusion);
    out_Final = vec4(vec3(occlusion), 1);
}
