#include <Bindings.glsl>
#include <Camera.glsl>
#include <Fog.glsl>
#include <FrameInfo.glsl>
#include <Random.glsl>

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
layout(binding = UBO_FOG_SETTINGS) uniform FogSettingsBlock
{
    FogSettings u_FogSettings;
};

layout(location = 0) in vec2 in_UV;

layout(location = 0) out vec4 out_Color;

void main()
{
    const mat4x4 invVP    = inverse(u_Camera.projection * u_Camera.view);
    const float backDepth = texture(u_Depth, in_UV)[0];
    const float stepSize  = 1 / float(FOG_STEPS);
    const float depthNoise = InterleavedGradientNoise(gl_FragCoord.xy, u_FrameInfo.frameIndex) * u_FogSettings.noiseDepthMultiplier;
    out_Color              = vec4(0);
    for (float i = 0; i < FOG_STEPS; i++) {
        const vec3 uv = vec3(in_UV, i * stepSize + depthNoise);
        if (uv.z >= backDepth)
            break;
        const vec3 NDCPos        = uv * 2.f - 1.f;
        const vec4 projPos       = (invVP * vec4(NDCPos, 1));
        const vec3 worldPos      = projPos.xyz / projPos.w;
        const float densityNoise = texture(u_FogNoise, worldPos * u_FogSettings.noiseDensityScale)[0] + (1 - u_FogSettings.noiseDensityIntensity);
        const vec4 fogColor      = texture(u_FogColor, vec3(uv.xy, pow(uv.z, FOG_DEPTH_EXP))) * vec4(1, 1, 1, min(densityNoise, 1.f));
        const float dist         = distance(u_Camera.position, worldPos);
        const float attenuation  = 1.0 - exp(-dist);
        out_Color                = mix(out_Color, fogColor, pow(attenuation, u_FogSettings.attenuationExp));
    }
    out_Color.a *= u_FogSettings.multiplier;
}
