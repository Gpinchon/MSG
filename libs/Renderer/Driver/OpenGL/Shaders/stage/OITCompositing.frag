#include <Bindings.glsl>
#include <Functions.glsl>
#include <OIT.glsl>

layout(binding = IMG_OIT_COLORS, rgba16f) restrict readonly uniform image3D img_Colors;
layout(binding = IMG_OIT_DEPTH, r32ui) restrict readonly uniform uimage3D img_Depth;

layout(location = OUTPUT_FRAG_FWD_COMP_COLOR) out vec4 out_Color;

void main()
{
    out_Color      = vec4(0);
    uint fragments = 0;
    for (uint i = 0; i < OIT_LAYERS && out_Color.a < 1; i++) {
        const ivec3 texCoord = ivec3(gl_FragCoord.xy, i);
        const uint zTest     = imageLoad(img_Depth, texCoord)[0];
        if (zTest != 0xFFFFFFFFu) { // we have something there !
            vec4 color = imageLoad(img_Colors, texCoord);
            color.rgb *= color.a;
            out_Color.rgb += (1 - out_Color.a) * color.rgb;
            out_Color.a += (1 - out_Color.a) * color.a;
        }
    }
}