#include <Bindings.glsl>
#include <Camera.glsl>
#include <Fog.glsl>
#include <FrameInfo.glsl>
#include <Random.glsl>

layout(binding = 0) uniform sampler3D u_FogScatteringExtinction;
layout(binding = 1) uniform sampler3D u_FogDensityNoise;
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

vec4 FogScatteredExtinction(IN(vec3) a_UVZ)
{
    return texture(u_FogScatteringExtinction, vec3(a_UVZ.xy, pow(a_UVZ.z, FOG_DEPTH_EXP)));
}

float BeerLaw(IN(float) a_Density, IN(float) a_StepSize)
{
    return pow(exp(-a_Density * a_StepSize), u_FogSettings.transmittanceExp);
}

vec3 GetWorldPos(IN(float) a_Depth)
{
    const mat4x4 invVP = inverse(u_Camera.projection * u_Camera.view);
    const vec3 uv      = vec3(in_UV, a_Depth);
    const vec3 NDCPos  = uv * 2.f - 1.f;
    const vec4 projPos = (invVP * vec4(NDCPos, 1));
    return projPos.xyz / projPos.w;
}

void main()
{
    const float screenStepSize = 1.f / float(textureSize(u_FogScatteringExtinction, 0).z);
    const float depthNoise     = InterleavedGradientNoise(gl_FragCoord.xy, u_FrameInfo.frameIndex) * screenStepSize;
    const float backDepth      = texture(u_Depth, in_UV)[0] + depthNoise;
    const vec3 uvz             = vec3(in_UV, backDepth);
    out_Color                  = FogScatteredExtinction(uvz);
}
