#include <Functions.glsl>
#include <ToneMapping.glsl>

layout(binding = 0, rgba16f) restrict readonly uniform image2D img_Color;
layout(binding = 1, rg16f) restrict readonly uniform image2D img_Velocity;

layout(binding = 0) uniform sampler2D u_Color_Previous;

layout(location = 0) in vec2 in_UV;
layout(location = 0) out vec4 out_Color;

const ivec2 neighborsOffset3x3[9] = ivec2[9](
    ivec2(-1, -1),  ivec2(0, -1),   ivec2(1, -1),
    ivec2(-1,  0),  ivec2(0,  0),   ivec2(1,  0),
    ivec2(-1,  1),  ivec2(0,  1),   ivec2(1,  1)
);

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


/**
 * @ref https://developer.download.nvidia.com/gameworks/events/GDC2016/msalvi_temporal_supersampling.pdf
 */
void main()
{
    const ivec2 colorSize = imageSize(img_Color);
    const ivec2 colorCoord = ivec2(in_UV * colorSize);
    const ivec2 velocitySize = imageSize(img_Velocity);
    const ivec2 velocityCoord = ivec2(in_UV * velocitySize);

    vec3 color    = vec3(0, 0, 0);
    vec2 velocity = vec2(0);
    vec3 min_3x3  = vec3(65504);
    vec3 max_3x3  = vec3(-65504);
    vec3 min_2x2  = vec3(65504);
    vec3 max_2x2  = vec3(-65504);
    for (uint i = 0; i < 9; ++i) {
        const vec3 colorSample = imageLoad(img_Color, colorCoord + neighborsOffset3x3[i]).rgb;
        const vec2 velocitySample = imageLoad(img_Velocity, velocityCoord + neighborsOffset3x3[i]).xy;
        if (length(velocity) < length(velocitySample))
            velocity = velocitySample;
        min_3x3 = min(min_3x3, colorSample);
        max_3x3 = max(max_3x3, colorSample);
        if (i % 2 == 0) { //We are at corners or middle
            min_2x2 = min(min_2x2, colorSample);
            max_2x2 = max(max_2x2, colorSample);
        }
        if (i == 4)
            color = colorSample;
    }
    const vec3 minColor = mix(min_3x3, min_2x2, 0.5);
    const vec3 maxColor = mix(max_3x3, max_2x2, 0.5);
    vec3 color_Previous = texture(u_Color_Previous, in_UV + velocity).rgb;
    color_Previous = clip_aabb(minColor, maxColor, color.rgb, color_Previous.rgb);
    //const float alpha = 0.90 + 0.10 * saturate(1 - abs(compMax(color) - compMax(color_Previous)));
    const float alpha = 0.90 + 0.10 * (1 - saturate(distance(color, color_Previous)));
    //Use rolling average over time
    //out = x * (1 - a) + y * a
    out_Color.rgb = mix(color.rgb, color_Previous, alpha);
    out_Color.a = 1;
}