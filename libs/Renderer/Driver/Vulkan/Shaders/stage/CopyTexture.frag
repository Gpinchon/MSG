layout(binding = 0) uniform sampler2D u_Source;

layout(location = 0) in vec2 in_UV;
layout(location = 0) out vec4 out_Color;

void main()
{
    out_Color = texture(u_Source, in_UV);
}
