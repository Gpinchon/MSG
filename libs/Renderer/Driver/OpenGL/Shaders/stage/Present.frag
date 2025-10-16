#include <ToneMapping.glsl>

layout(binding = 0, rgba16f) restrict readonly uniform image2D img_input;
layout(binding = 1, rgba8) restrict writeonly uniform image2D img_output;

layout(location = 0) in vec2 in_UV;

void main()
{
    ivec2 coord_input  = ivec2(in_UV * imageSize(img_input));
    ivec2 coord_output = ivec2(in_UV * imageSize(img_output));
    vec4 inColor       = imageLoad(img_input, coord_input);
    vec3 color         = ReinhardTonemapping(inColor.rgb, 1.5f);
    color              = CZMSaturation(color, 2.f);
    imageStore(img_output, coord_output, vec4(color, 1) * inColor.a);
}
