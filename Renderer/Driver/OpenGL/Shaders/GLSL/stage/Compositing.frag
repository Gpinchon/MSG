#include <Bindings.glsl>
#include <Functions.glsl>

layout(binding = 0, rgba16f) restrict readonly uniform image2D img_accum;
layout(binding = 1, r8) restrict readonly uniform image2D img_revealage;

layout(location = OUTPUT_FRAG_FWD_COMP_COLOR) out vec4 out_Color;

void main() {
    ivec2 coord = ivec2(gl_FragCoord.xy);
    float revealage = imageLoad(img_revealage, coord).r;
    if (revealage == 1.0) {
        // Save the blending and color texture fetch cost
        discard;
    }
    vec4 accum     = imageLoad(img_accum, coord);

    // Suppress overflow
    if (isinf(compMax(abs(accum)))) {
        accum.rgb = vec3(accum.a);
    }

    // dst' =  (accum.rgb / accum.a) * (1 - revealage) + dst
    // [dst has already been modulated by the transmission colors and coverage and the blend mode
    // inverts revealage for us] 
    out_Color = vec4(accum.rgb / max(accum.a, 0.00001), revealage);
}