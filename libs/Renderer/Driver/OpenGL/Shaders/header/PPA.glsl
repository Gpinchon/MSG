#ifndef PPA_GLSL
#define PPA_GLSL

#define PPA_LAYERS 5

/**
 * @brief this alpha blending implementation was strongly inspired
 * by the work of mklefrancois & al. down at https://github.com/nvpro-samples/vk_order_independent_transparency
 * cudos to them for making such an invaluable resource!
 */

#ifndef __cplusplus
#include <Functions.glsl>

vec4 PPAUnpackColor(IN(uvec4) a_Data)
{
    vec4 color       = unpackUnorm4x8(a_Data[0]);
    float multiplier = unpackHalf2x16(a_Data[1])[0];
    return color * multiplier;
}

float PPAUnpackDepth(IN(uvec4) a_Data)
{
    float depth = unpackHalf2x16(a_Data[1])[1];
    return remap(depth, -65504, 65504, 0, 1);
}

uvec4 PPAPackElement(IN(vec4) a_Color, IN(float) a_Depth)
{
    float colorMultiplier = compMax(a_Color);
    return uvec4(
        packUnorm4x8(a_Color / colorMultiplier),
        packHalf2x16(vec2(colorMultiplier, remap(a_Depth, 0, 1, -65504, 65504))),
        0, 0 // padding
    );
}
#endif
#endif // PPA_GLSL