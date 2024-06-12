#version 430

layout(location = 0) out vec4 out_Color;
layout(binding = 0) uniform sampler2D in_Color;

in vec2 UV;

void main()
{
    ivec2 coord = ivec2(UV * textureSize(in_Color, 0));
    out_Color   = texelFetch(in_Color, coord, 0);
    // out_Color = vec4(UV, 0, 1);
}
