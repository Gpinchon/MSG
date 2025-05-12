#include <GaussianBlur.glsl>
#include <Bicubic.glsl>

layout(binding = 0) uniform sampler2D u_Color;

layout(location = 0) in invariant vec2 in_UV;

layout(location = 0) out vec4 out_Color0;
layout(location = 1) out vec4 out_Color1;

layout(binding = 0) uniform SettingsBlock
{
    GaussianBlurSettings u_Settings;
};

const float weight[5] = float[5](0.16, 0.15, 0.12, 0.09, 0.05);

void main()
{
    const vec2 offset = (vec2(1.f / textureSize(u_Color, 0).xy) * u_Settings.direction * u_Settings.scale) / float(weight.length());
	vec4 outColor     = textureNice(u_Color, in_UV) * weight[0];
    for (uint i = 1; i < weight.length(); ++i) {
        const vec2 UV0 = in_UV + (offset * float(i));
        const vec2 UV1 = in_UV - (offset * float(i));
        outColor += textureNice(u_Color, UV0) * weight[i];
        outColor += textureNice(u_Color, UV1) * weight[i];
    }
	out_Color0 = out_Color1 = outColor;
}
