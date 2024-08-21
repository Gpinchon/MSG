layout(location = 0) out vec4 out_Color;
layout(binding = 0) uniform sampler2D in_Color;

layout(location = 0) in vec2 in_UV;

void main()
{
    ivec2 coord = ivec2(in_UV * textureSize(in_Color, 0));
    out_Color   = texelFetch(in_Color, coord, 0);
}
