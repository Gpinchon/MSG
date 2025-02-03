#include <Functions.glsl>
#include <ToneMapping.glsl>

layout(binding = 0) uniform sampler2D u_Color_Previous;
layout(binding = 1) uniform sampler2D u_Color;
layout(binding = 2) uniform sampler2D u_Velocity;

layout(location = 0) in vec2 in_UV;
layout(location = 0) out vec4 out_Color;

#define SAMPLE_COUNT            9
#define CLIPPING_VARIANCE_GAMMA 1.0f
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
vec3 ClipAABB(vec3 a_AABBMin, vec3 a_AABBMax, in vec3 a_Color, in vec3 a_ColorPrev)
{
    vec3 p_clip   = 0.5 * (a_AABBMax + a_AABBMin);
    vec3 e_clip   = 0.5 * (a_AABBMax - a_AABBMin);
    vec3 v_clip   = a_ColorPrev - p_clip;
    vec3 v_unit   = v_clip.xyz / e_clip;
    vec3 a_unit   = abs(v_unit);
    float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));
    if (ma_unit > 1.f)
        return p_clip + v_clip / ma_unit;
    else
        return a_ColorPrev; // point inside aabb
}

/**
 * @ref https://developer.download.nvidia.com/gameworks/events/GDC2016/msalvi_temporal_supersampling.pdf
 */
void main()
{
    out_Color                 = vec4(0, 0, 0, 1);
    const ivec2 colorSize     = textureSize(u_Color, 0);
    const ivec2 colorCoord    = ivec2(in_UV * colorSize);
    const ivec2 velocitySize  = textureSize(u_Velocity, 0);
    const ivec2 velocityCoord = ivec2(in_UV * velocitySize);

    vec3 color    = vec3(0);
    vec2 velocity = vec2(0);
#if CLIPPING == CLIPPING_VARIANCE
    vec3 m1 = vec3(0);
    vec3 m2 = vec3(0);
#elif CLIPPING == CLIPPING_RGB
    vec3 minC = vec3(65504);
    vec3 maxC = vec3(-65504);
#endif

    for (uint i = 0; i < SAMPLE_COUNT; ++i) {
        const vec2 colorUV        = (colorCoord + neighborsOffset3x3[i]) / vec2(colorSize);
        const vec3 colorSample    = texture(u_Color, colorUV).rgb;
        const vec2 velocityUV     = (velocityCoord + neighborsOffset3x3[i]) / vec2(colorSize);
        const vec2 velocitySample = texture(u_Velocity, velocityUV).xy;
        if (i == SAMPLE_COUNT / 2)
            color = colorSample;

#if CLIPPING == CLIPPING_VARIANCE
        m1 += (colorSample);
        m2 += (colorSample * colorSample);
#elif CLIPPING == CLIPPING_RGB
        minC = min(minC, colorSample);
        maxC = max(maxC, colorSample);
#endif
        if (length(velocity) < length(velocitySample))
            velocity = velocitySample;
    }
    vec3 colorPrev = texture(u_Color_Previous, in_UV + velocity).rgb;
#if CLIPPING == CLIPPING_VARIANCE
    vec3 mu       = m1 / float(SAMPLE_COUNT);
    vec3 sigma    = sqrt(abs(m2 / float(SAMPLE_COUNT) - mu * mu));
    vec3 minC     = mu - CLIPPING_VARIANCE_GAMMA * sigma;
    vec3 maxC     = mu + CLIPPING_VARIANCE_GAMMA * sigma;
    out_Color.rgb = ClipAABB(minC, maxC, color, colorPrev);
#elif CLIPPING == CLIPPING_RGB
    out_Color.rgb = ClipAABB(minC, maxC, color, colorPrev);
#endif
}