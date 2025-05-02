#include <Bindings.glsl>
#include <Functions.glsl>
#include <PPA.glsl>

layout(binding = IMG_PPA_ARRAY, rgba32ui) restrict readonly uniform uimage3D img_Arrays;
layout(binding = IMG_PPA_COUNTER, r8ui) restrict readonly uniform uimage2D img_Counters;

layout(location = OUTPUT_FRAG_FWD_COMP_COLOR) out vec4 out_Color;

void BubbleSort(inout uvec4 array[PPA_LAYERS], int n)
{
#if PPA_LAYERS > 1
    for (int i = (n - 2); i >= 0; --i) {
        for (int j = 0; j <= i; ++j) {
            if (uintBitsToFloat(array[j][2]) >= uintBitsToFloat(array[j + 1][2])) {
                // Swap array[j] and array[j+1]
                uvec4 temp   = array[j + 1];
                array[j + 1] = array[j];
                array[j]     = temp;
            }
        }
    }
#endif
}

void main()
{
    int counter = int(imageLoad(img_Counters, ivec2(gl_FragCoord.xy))[0]);
    uvec4 elems[PPA_LAYERS];
    for (int i = 0; i < counter; i++)
        elems[i] = imageLoad(img_Arrays, ivec3(gl_FragCoord.xy, i));
    BubbleSort(elems, counter);
    out_Color = vec4(0);
    for (int i = 0; i < counter; i++) {
        vec4 color = PPAUnpackColor(elems[i]);
        color.rgb *= color.a;
        out_Color.rgb += (1 - out_Color.a) * color.rgb;
        out_Color.a += (1 - out_Color.a) * color.a;
    }
}