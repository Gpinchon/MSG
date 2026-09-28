#include <Bicubic.glsl>
#include <Functions.glsl>
#include <YCoCg.glsl>

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

const ivec2 neighborsOffset3x3[8] = ivec2[8](
    ivec2(-1, -1), ivec2(0, -1), ivec2(1, -1),
    ivec2(-1, 0), /*ivec2(0, 0),*/ ivec2(1, 0),
    ivec2(-1, 1), ivec2(0, 1), ivec2(1, 1));

/**
 * @ref http://s3.amazonaws.com/arena-attachments/655504/c5c71c5507f0f8bf344252958254fb7d.pdf?1468341463
 */
vec4 ClipAABB(vec4 a_AABBMin, vec4 a_AABBMax, in vec4 a_Color, in vec4 a_ColorPrev)
{
    vec4 p_clip   = 0.5 * (a_AABBMax + a_AABBMin);
    vec4 e_clip   = 0.5 * (a_AABBMax - a_AABBMin);
    vec4 v_clip   = a_ColorPrev - p_clip;
    vec3 v_unit   = v_clip.rgb / (e_clip.rgb + EPSILON);
    vec3 a_unit   = abs(v_unit);
    float ma_unit = compMax(a_unit);

    // if ma_unit > 1 we're outside, return clipped color
    // branchless version of ma_unit > 1.f ? clipped_color : a_ColorPrev
    float is_outside   = step(1.f, ma_unit);
    vec4 clipped_color = p_clip + v_clip / (ma_unit + EPSILON);
    return mix(a_ColorPrev, clipped_color, is_outside);
}

/**
 * @ref https://developer.download.nvidia.com/gameworks/events/GDC2016/msalvi_temporal_supersampling.pdf
 */
void main()
{
    out_Color              = vec4(0, 0, 0, 1);
    const ivec2 colorSize  = textureSize(u_Color, 0);
    const ivec2 colorCoord = ivec2(in_UV * colorSize);
    const vec4 color       = texelFetch(u_Color, colorCoord, 0);

#if CLIPPING == CLIPPING_VARIANCE
    const vec4 colorYCoCgA = RGBA2YCoCgA(color);
    vec4 m1                = colorYCoCgA;
    vec4 m2                = colorYCoCgA * colorYCoCgA;
#elif CLIPPING == CLIPPING_RGB
    vec4 minC = color;
    vec4 maxC = color;
#endif

    for (uint i = 0; i < neighborsOffset3x3.length(); ++i) {
        const ivec2 colorTexCoord = colorCoord + neighborsOffset3x3[i];
        const vec4 colorSample    = texelFetch(u_Color, colorTexCoord, 0);
#if CLIPPING == CLIPPING_VARIANCE
        const vec4 colorSampleYCoCgA = RGBA2YCoCgA(colorSample);
        m1 += colorSampleYCoCgA;
        m2 += colorSampleYCoCgA * colorSampleYCoCgA;
#elif CLIPPING == CLIPPING_RGB
        minC = min(minC, colorSample);
        maxC = max(maxC, colorSample);
#endif
    }
    const vec2 velocity     = textureBicubic(u_Velocity, in_UV).xy;
    const vec2 uv_Prev      = in_UV + velocity;
    const vec4 colorPrev    = textureBicubic(u_Color_Previous, uv_Prev);
    const bool sampleInside = all(greaterThanEqual(uv_Prev, vec2(0))) && all(lessThanEqual(uv_Prev, vec2(1)));
#if CLIPPING == CLIPPING_VARIANCE
    const vec4 mu           = m1 / float(SAMPLE_COUNT);
    const vec4 variance     = sqrt(abs(m2 / float(SAMPLE_COUNT) - mu * mu)) * CLIPPING_VARIANCE_GAMMA;
    const vec4 minC         = mu - variance;
    const vec4 maxC         = mu + variance;
    const vec4 clippedColor = YCoCgA2RGBA(ClipAABB(minC, maxC, colorYCoCgA, sampleInside ? RGBA2YCoCgA(colorPrev) : colorYCoCgA));
#elif CLIPPING == CLIPPING_RGB
    const vec4 clippedColor = ClipAABB(minC, maxC, color, colorPrev);
#endif
    out_Color = mix(clippedColor, color, INTEGRATION_WEIGHT);
}