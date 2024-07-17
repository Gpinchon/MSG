layout(binding = 0, rgba16f) readonly uniform image2D img_input;
layout(binding = 1, rgba8) writeonly uniform image2D img_output;

in vec2 UV;

void main()
{
    ivec2 coord = ivec2(gl_FragCoord.xy);
    imageStore(img_output, coord, imageLoad(img_input, coord));
}
