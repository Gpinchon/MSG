#include <GaussianBlur.glsl>

layout(binding = 0) uniform sampler2D u_Color;

layout(location = 0) in invariant vec2 in_UV;

layout(location = 0) out vec4 out_Color0;
layout(location = 1) out vec4 out_Color1;

layout(binding = 0) uniform SettingsBlock
{
    GaussianBlurSettings u_Settings;
};

void main()
{
    out_Color0 = out_Color1 = GaussianBlur(u_Color, in_UV, u_Settings);
}
