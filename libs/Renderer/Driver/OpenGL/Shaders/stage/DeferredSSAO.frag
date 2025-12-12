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
#define SAMPLENBR 8
#elif SSAO_QUALITY == 2
#define SAMPLENBR 16
#elif SSAO_QUALITY == 3
#define SAMPLENBR 32
#else // SSAO_QUALITY == 4
#define SAMPLENBR 64
#endif

vec3 GetWorldPosition(IN(vec2) a_UV, IN(mat4x4) a_InvVP)
{
    GBufferDataPacked gbufferDataPacked;
    gbufferDataPacked.data0 = imageLoad(img_GBuffer0, ivec2(a_UV * imageSize(img_GBuffer0)));
    gbufferDataPacked.data1 = imageLoad(img_GBuffer1, ivec2(a_UV * imageSize(img_GBuffer1)));
    GBufferData gBufferData = UnpackGBufferData(gbufferDataPacked);
    const vec3 NDCPos       = vec3(a_UV * 2.f - 1.f, gBufferData.ndcDepth);
    const vec4 projPos      = a_InvVP * vec4(NDCPos, 1);
    return projPos.xyz / projPos.w;
}

vec3 SampleHemisphere_Uniform(IN(uint) i, IN(uint) numSamples, IN(uvec2) a_RandBase)
{
    // Returns a 3D sample vector orientated around (0.0, 1.0, 0.0)
    // For practical use, must rotate with a rotation matrix (or whatever
    // your preferred approach is) for use with normals, etc.
    vec2 xi        = Hammersley16(i, numSamples, a_RandBase);
    float phi      = xi.y * 2.0 * M_PI;
    float cosTheta = 1.0 - xi.x;
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    return vec3(cos(phi) * sinTheta, cosTheta, sin(phi) * sinTheta);
}

void main()
{
    ivec2 texCoord = ivec2(gl_FragCoord.xy);
    GBufferDataPacked gbufferDataPacked;
    gbufferDataPacked.data0 = imageLoad(img_GBuffer0, texCoord);
    gbufferDataPacked.data1 = imageLoad(img_GBuffer1, texCoord);
    GBufferData gBufferData = UnpackGBufferData(gbufferDataPacked);

    const uvec2 rand   = Rand3DPCG16(ivec3(gl_FragCoord.xy, u_FrameInfo.frameIndex)).xy;
    const mat4x4 VP    = u_Camera.projection * u_Camera.view;
    const mat4x4 invVP = inverse(VP);
    const vec3 NDCPos  = vec3(in_UV * 2.f - 1.f, gBufferData.ndcDepth);
    const vec4 projPos = invVP * vec4(NDCPos, 1);
    const vec3 P       = projPos.xyz / projPos.w;
    const vec3 N       = gBufferData.normal;

    float occlusion = 0.f;
    for (int i = 0; i < SAMPLENBR; ++i) {
        const float bias   = Hammersley16(i, SAMPLENBR, rand)[0];
        const vec3 Ni      = normalize(N + SampleHemisphere_Uniform(i, SAMPLENBR, rand));
        const vec3 Pi      = P + Ni * u_SSAOSettings.radius * bias;
        const vec4 NDCProj = VP * vec4(Pi, 1);
        vec2 uv            = (NDCProj.xy / NDCProj.w) * 0.5 + 0.5;
        if (any(lessThan(uv, vec2(0))) || any(greaterThan(uv, vec2(1))))
            continue;
        vec3 V                = GetWorldPosition(uv, invVP) - P;
        const float dist      = length(V);
        V                     = V / dist;
        const float intensity = normalizeValue(dist, 0, u_SSAOSettings.radius);
        occlusion += step(0.2, dot(N, V)) * (1 - intensity);
    }
    occlusion /= float(SAMPLENBR);
    occlusion *= u_SSAOSettings.strength;
    occlusion = saturate(gBufferData.AO * (1 - occlusion));
    out_Final = vec4(vec3(occlusion), 1);
}