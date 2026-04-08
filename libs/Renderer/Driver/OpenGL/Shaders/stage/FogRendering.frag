#include <Bindings.glsl>
#include <FrameInfo.glsl>
#include <Random.glsl>

layout(binding = UBO_FRAME_INFO) uniform FrameInfoBlock
{
    FrameInfo u_FrameInfo;
};

layout(binding = 0) uniform sampler3D u_FogScatteringExtinction;

layout(location = 0) in vec2 in_UV;

layout(location = 0) out vec4 out_Color;

void main()
{
    const vec3 resultSize = textureSize(u_FogScatteringExtinction, 0);
    const int haltonIndex = int(u_FrameInfo.frameIndex + texCoord.x + texCoord.y * 2.f);
    const vec3 jitter     = (Halton235(haltonIndex) - 0.5f) / resultSize;
    out_Color             = texture(u_FogScatteringExtinction, vec3(in_UV, 1) * jitter);
}
