#include <ToneMapping.glsl>

layout(binding = 0, rgba16f) restrict readonly uniform image2D img_input;
layout(binding = 1, rgba8) restrict writeonly uniform image2D img_output;

in vec2 UV;

void main()
{
    ivec2 coord = ivec2(gl_FragCoord.xy);
    vec3 color  = ReinhardTonemapping(imageLoad(img_input, coord).rgb);
    color       = CZMSaturation(color, 2.f);
    imageStore(img_output, coord, vec4(color, 1.f));
}
