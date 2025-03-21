#include <FogCulling.glsl>
#include <Random.glsl>
#include <Bindings.glsl>
#include <FrameInfo.glsl>
#include <Camera.glsl>

layout(binding = 0) uniform sampler3D u_FogColor;
layout(binding = 1) uniform sampler3D u_FogNoise;
layout(binding = 2) uniform sampler2D u_Depth;

layout(binding = UBO_FRAME_INFO) uniform FrameInfoBlock
{
    FrameInfo u_FrameInfo;
};
layout(binding = UBO_CAMERA) uniform CameraBlock
{
    Camera u_Camera;
};

layout(location = 0) in vec2 in_UV;

layout(location = 0) out vec4 out_Color;

#define DEPTH_NOISE_INTENSITY   0.1f
#define DENSITY_NOISE_INTENSITY 1.0f
#define DENSITY_NOISE_SCALE     0.1f

void main()
{
    const mat4x4 invVP     = inverse(u_Camera.projection * u_Camera.view);
    const float backDepth  = texture(u_Depth, in_UV)[0];
    const float stepSize   =  1 / float(FOG_STEPS);
    const float depthNoise = InterleavedGradientNoise(gl_FragCoord.xy, u_FrameInfo.frameIndex) * DEPTH_NOISE_INTENSITY;
    out_Color              = vec4(0);
    for (
        vec3 uv = vec3(in_UV, depthNoise);
        uv.z < 1 && uv.z < backDepth && out_Color.a < 1;
        uv.z += stepSize)
    {
        const vec3 NDCPos        = uv * 2.f - 1.f;
        const vec4 projPos       = (invVP * vec4(NDCPos, 1));
        const vec3 worldPos      = projPos.xyz / projPos.w;
        const float densityNoise = texture(u_FogNoise, worldPos * DENSITY_NOISE_SCALE)[0] * DENSITY_NOISE_INTENSITY;
        vec4 fogColor            = texture(u_FogColor, uv);
        const float fogDensity   = saturate(fogColor.a - densityNoise);
        fogColor.a               = fogColor.a * fogDensity;
        //out_Color                = out_Color + fogColor * stepSize;
        out_Color = mix(out_Color, fogColor, uv.z);
    }
}
