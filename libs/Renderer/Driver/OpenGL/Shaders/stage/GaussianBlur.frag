#include <GaussianBlur.glsl>

layout(binding = 0) uniform sampler2D u_Color;

layout(location = 0) in invariant vec2 in_UV;

layout(location = 0) out vec4 out_Color0;
layout(location = 1) out vec4 out_Color1;

layout(binding = 0) uniform SettingsBlock
{
    GaussianBlurSettings u_Settings;
};

const float weight[3] = float[3](
    0.36340823233342295,
    0.35898644860837337,
    0.2776053190582037);

/*
const float weight[5] = float[5](
    0.23859562102626392,
    0.25106008818678394,
    0.22531141067113736,
    0.17245492039515026,
    0.11257795972066456);
*/
/*
const float weight[5] = float[5](
    0.250301f,
    0.221461f,
    0.221461f,
    0.153388f,
    0.153388f);
*/

void main()
{
    vec2 offset = (vec2(1.f / textureSize(u_Color, 0).xy) * u_Settings.direction * u_Settings.scale) / float(weight.length());
	out_Color0   = texture(u_Color, in_UV) * weight[0] * 0.5f;
    for (uint i = 1; i < weight.length(); ++i) {
        const vec2 UV0 = in_UV + (offset * float(i));
        const vec2 UV1 = in_UV - (offset * float(i));
        out_Color0 += texture(u_Color, UV0) * weight[i] * 0.5f;
        out_Color0 += texture(u_Color, UV1) * weight[i] * 0.5f;
    }
	out_Color1 = out_Color0;
}
