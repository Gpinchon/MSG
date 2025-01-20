layout (binding = 0) uniform sampler2D u_Color;
layout (binding = 1) uniform sampler2D u_Scale;

layout(location = 0) in invariant vec2 in_UV;

layout(location = 0) out vec4 out_Color;

layout(binding = 0) uniform SettingsBlock {
    vec2 u_Direction;
};

const float weight[5] = float[5](
    0.250301f,
    0.221461f,
    0.221461f,
    0.153388f,
    0.153388f
);

void main()
{
    out_Color   = vec4(0);
    vec2 scale  = texture(u_Scale, in_UV).xy;
    vec2 offset = 1.f / textureSize(u_Color, 0) * u_Direction * scale;
    for (uint i = 0; i < 5; ++i) {
        const vec2 UV0 = (in_UV + offset) * i;
        const vec2 UV1 = (in_UV - offset) * i;
        out_Color += texture(u_Color, UV0) * weight[i];
        out_Color += texture(u_Color, UV1) * weight[i];
    }
}
