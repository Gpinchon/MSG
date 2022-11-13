R""(
layout(location = 0) out vec4 out_0;

uniform sampler2D   in_PreviousColor;
uniform sampler2D	in_CurrentColor;
uniform sampler2D   in_CurrentVelocity;
uniform vec3        Resolution;

#define ScreenTexCoord() (gl_FragCoord.xy / Resolution.xy)
#define Luminance(linearColor) dot(linearColor, vec3(0.299, 0.587, 0.114))

/**
 * @ref http://s3.amazonaws.com/arena-attachments/655504/c5c71c5507f0f8bf344252958254fb7d.pdf?1468341463
 */
vec3 clip_aabb(vec3 aabb_min, vec3 aabb_max, vec3 p, vec3 q)
{
	vec3 p_clip = 0.5 * (aabb_max + aabb_min);
	vec3 e_clip = 0.5 * (aabb_max - aabb_min);
	vec3 v_clip = q - p_clip;
	vec3 v_unit = v_clip.xyz / e_clip;
	vec3 a_unit = abs(v_unit);
	float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));
	if (ma_unit > 1.0)
		return vec3(p_clip) + v_clip / ma_unit;
	else
		return q;// point inside aabb
}

const vec2 neighborsOffset3x3[9] = vec2[9](
    vec2(-1, -1),	vec2(0, -1),	vec2(1, -1),
    vec2(-1,  0),	vec2(0,  0),	vec2(1,  0),
    vec2(-1,  1),	vec2(0,  1),	vec2(1,  1)
);

/**
 * @ref https://developer.download.nvidia.com/gameworks/events/GDC2016/msalvi_temporal_supersampling.pdf
 */
void TemporalAccumulation()
{
    out_0 = vec4(0, 0, 0, 1);
    const vec2 pixSize = 1.f / textureSize(in_CurrentColor, 0);
    vec2 velocity = vec2(0);
    vec3 min_3x3 = vec3(1);
    vec3 max_3x3 = vec3(0);
    vec3 min_2x2 = vec3(1);
    vec3 max_2x2 = vec3(0);
    for (uint i = 0; i < 9; ++i) {
        vec2 newUV = ScreenTexCoord() + (neighborsOffset3x3[i] * pixSize);
        vec3 color = texture(in_CurrentColor, newUV).rgb;
        vec2 velocitySample = texelFetch(in_CurrentVelocity, ivec2(ScreenTexCoord() * textureSize(in_CurrentVelocity, 0) + neighborsOffset3x3[i]), 0).xy;
        if (length(velocity) < length(velocitySample))
            velocity = velocitySample;
        min_3x3 = min(min_3x3, color);
        max_3x3 = max(max_3x3, color);
        if (i % 2 == 0) { //We are at corners or middle
            min_2x2 = min(min_2x2, color);
            max_2x2 = max(max_2x2, color);
        }
        if (i == 4)
            out_0.rgb = color;
    }
    const vec2 historyUV = ScreenTexCoord() + velocity;
    vec3 historyColor = texture(in_PreviousColor, historyUV).rgb;
    if (any(lessThan(historyUV, vec2(0))) || any(greaterThan(historyUV, vec2(1))))
        return;
    vec3 minColor = mix(min_3x3, min_2x2, 0.5);
    vec3 maxColor = mix(max_3x3, max_2x2, 0.5);
    historyColor.rgb = clip_aabb(minColor, maxColor, out_0.rgb, historyColor.rgb);
    float alpha = 0.9 + 0.05 * (1 - abs(Luminance(historyColor.rgb) - Luminance(out_0.rgb)));
    //Use rolling average over time
    //out = x * (1 - a) + y * a
    out_0.rgb = mix(out_0.rgb, historyColor, alpha);
}

)""
