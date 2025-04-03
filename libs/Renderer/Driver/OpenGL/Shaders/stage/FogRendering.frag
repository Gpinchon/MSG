#include <Bindings.glsl>
#include <Camera.glsl>
#include <Fog.glsl>
#include <FrameInfo.glsl>
#include <Random.glsl>

layout(binding = 0) uniform sampler3D u_FogScatteringExtinction;

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
    out_Color = texture(u_FogScatteringExtinction, vec3(in_UV, 1));
}
