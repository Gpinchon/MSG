#ifndef PPA_GLSL
#define PPA_GLSL

#define PPA_LAYERS 10

/**
 * @brief this alpha blending implementation was strongly inspired
 * by the work of mklefrancois & al. down at https://github.com/nvpro-samples/vk_order_independent_transparency
 * cudos to them for making such an invaluable resource!
 */

#ifndef __cplusplus
vec4 PPAUnpackColor(IN(uvec4) a_Color)
{
    return vec4(
        unpackHalf2x16(a_Color[0]),
        unpackHalf2x16(a_Color[1]));
}

uvec2 PPAPackColor(IN(vec4) a_Color)
{
    return uvec2(
        packHalf2x16(a_Color.rg),
        packHalf2x16(a_Color.ba));
}

uvec4 PPAPackElement(IN(vec4) a_Color, IN(float) a_Depth)
{
    return uvec4(
        PPAPackColor(a_Color),
        floatBitsToUint(a_Depth),
        0 // padding
    );
}
#endif
#endif // PPA_GLSL