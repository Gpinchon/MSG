#include <Bicubic.glsl>
#include <Functions.glsl>
#include <ToneMapping.glsl>

layout(binding = 0) uniform sampler2D u_Color_Previous;
layout(binding = 1) uniform sampler2D u_Color;
layout(binding = 2) uniform sampler2D u_Velocity;

layout(location = 0) in vec2 in_UV;
layout(location = 0) out vec4 out_Color;

#define SAMPLE_COUNT            9
#define INTEGRATION_WEIGHT      0.05f /* weight of the current frame */
#define CLIPPING_VARIANCE_GAMMA 1.5f
#define CLIPPING_VARIANCE       0
#define CLIPPING_RGB            1
#define CLIPPING                CLIPPING_VARIANCE

const ivec2 neighborsOffset3x3[9] = ivec2[9](
    ivec2(-1, -1), ivec2(0, -1), ivec2(1, -1),
    ivec2(-1, 0), ivec2(0, 0), ivec2(1, 0),
    ivec2(-1, 1), ivec2(0, 1), ivec2(1, 1));

/**
 * @ref http://s3.amazonaws.com/arena-attachments/655504/c5c71c5507f0f8bf344252958254fb7d.pdf?1468341463
 */
vec4 ClipAABB(vec4 a_AABBMin, vec4 a_AABBMax, in vec4 a_Color, in vec4 a_ColorPrev)
{
    vec4 p_clip   = 0.5 * (a_AABBMax + a_AABBMin);
    vec4 e_clip   = 0.5 * (a_AABBMax - a_AABBMin);
    vec4 v_clip   = a_ColorPrev - p_clip;
    vec4 v_unit   = v_clip / e_clip;
    vec4 a_unit   = abs(v_unit);
    float ma_unit = compMax(a_unit);
    if (isnan(ma_unit))
        return a_Color;
    else if (ma_unit > 1.f)
        return p_clip + v_clip / ma_unit;
    else
        return a_ColorPrev; // point inside aabb
}

/**
 * @ref https://developer.download.nvidia.com/gameworks/events/GDC2016/msalvi_temporal_supersampling.pdf
 */
void main()
{
    out_Color              = vec4(0, 0, 0, 1);
    const ivec2 colorSize  = textureSize(u_Color, 0);
    const ivec2 colorCoord = ivec2(in_UV * colorSize);

#if CLIPPING == CLIPPING_VARIANCE
    vec4 m1 = vec4(0);
    vec4 m2 = vec4(0);
#elif CLIPPING == CLIPPING_RGB
    vec4 minC = vec4(65504);
    vec4 maxC = vec4(-65504);
#endif
    vec4 color = vec4(0);
    for (uint i = 0; i < SAMPLE_COUNT; ++i) {
        const ivec2 colorTexCoord = colorCoord + neighborsOffset3x3[i];
        const vec4 colorSample    = texelFetch(u_Color, colorTexCoord, 0);
        if (i == SAMPLE_COUNT / 2)
            color = colorSample;
#if CLIPPING == CLIPPING_VARIANCE
        m1 += (colorSample);
        m2 += (colorSample * colorSample);
#elif CLIPPING == CLIPPING_RGB
        minC = min(minC, colorSample);
        maxC = max(maxC, colorSample);
#endif
    }
    const vec2 velocity  = textureBicubic(u_Velocity, in_UV).xy;
    const vec4 colorPrev = textureBicubic(u_Color_Previous, in_UV + velocity);
#if CLIPPING == CLIPPING_VARIANCE
    const vec4 mu           = m1 / float(SAMPLE_COUNT);
    const vec4 sigma        = sqrt(abs(m2 / float(SAMPLE_COUNT) - mu * mu));
    const vec4 minC         = mu - CLIPPING_VARIANCE_GAMMA * sigma;
    const vec4 maxC         = mu + CLIPPING_VARIANCE_GAMMA * sigma;
    const vec4 clippedColor = ClipAABB(minC, maxC, color, colorPrev);
#elif CLIPPING == CLIPPING_RGB
    const vec4 clippedColor = ClipAABB(minC, maxC, color, colorPrev);
#endif
    out_Color = mix(clippedColor, color, INTEGRATION_WEIGHT);
}