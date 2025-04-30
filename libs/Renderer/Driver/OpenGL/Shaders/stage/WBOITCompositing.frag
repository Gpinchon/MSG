#include <Bindings.glsl>
#include <Functions.glsl>

layout(binding = IMG_WBOIT_ACCUM, rgba16f) restrict readonly uniform image3D img_accum;
layout(binding = IMG_WBOIT_REV, r8) restrict readonly uniform image3D img_revealage;

layout(location = OUTPUT_FRAG_FWD_COMP_COLOR) out vec4 out_Color;

void main()
{
    vec3 cDst  = vec3(0, 0, 0);
    float aDst = 1;
    for (int layerIndex = 0; layerIndex < imageSize(img_accum).z; layerIndex++) {
        ivec3 coord     = ivec3(gl_FragCoord.xy, layerIndex);
        float revealage = imageLoad(img_revealage, coord).r;
        if (revealage == 1.0) {
            // Save the blending and color texture fetch cost
            continue;
        }
        vec4 accum = imageLoad(img_accum, coord);

        // Suppress overflow
        if (isinf(compMax(abs(accum)))) {
            accum.rgb = vec3(accum.a);
        }

        // dst' =  (accum.rgb / accum.a) * (1 - revealage) + dst
        // [dst has already been modulated by the transmission colors and coverage and the blend mode
        // inverts revealage for us]
        vec3 layerC  = accum.rgb / max(accum.a, 0.00001);
        float layerA = 1 - revealage;
        cDst         = aDst * (layerC * layerA) + cDst;
        aDst         = (1 - layerA) * aDst;
    }
    out_Color = vec4(cDst, aDst);
}