R""(
layout(location = 0) out vec3	out_ColorBuffer0;

uniform sampler2D	in_TransparentColor; //Transp Color
uniform sampler2D	in_TransparentAlphaCoverage; //Transp Coverage
uniform sampler2D	in_TransparentDistortion; //Transp Distortion
uniform sampler2D	in_TransparentTransmission;

uniform sampler2D	in_OpaqueColor; //Opaque Color

in vec2 frag_UV;

float maxComponent(const in vec4 v) {
    return max(max(v.x, v.y), max(v.z, v.w));
}

float	warpUV(float min, float max, float percent)
{
	if (percent < 0 || percent > 1)
		return (smoothstep(min, max, 1 - mod(percent, 1)));
	return (percent);
}

vec2	warpUV(vec2 min, vec2 max, vec2 percent)
{
	return (vec2(warpUV(min.x, max.x, percent.x), warpUV(min.y, max.y, percent.y)));
}

vec4 BlendColor(vec4 s, vec4 d) {
	//glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
	//glBlendEquation(GL_FUNC_ADD);
	return (s*(1-s.a)) + (d*s.a);
}

#ifndef textureQueryLevels
float compMax(vec3 v) { return max(max(v.x, v.y), v.z); }
float compMax(vec2 v) { return max(v.x, v.y); }
#define textureQueryLevels(tex) int(1 + log2(compMax(textureSize(tex, 0))))
#endif

#define sampleLod(tex, uv, lod) textureLod(tex, uv, lod * textureQueryLevels(tex))

void Composite()
{
	vec4 transmissionColor = texture(in_TransparentTransmission, frag_UV, 0);
    vec4 transparentRefract =  texture(in_TransparentDistortion, frag_UV, 0);
    vec2 refractUV = warpUV(vec2(0), vec2(1), frag_UV + transparentRefract.xy);

	vec4 accum = texture(in_TransparentColor, frag_UV, 0);
    float coverage = texture(in_TransparentAlphaCoverage, frag_UV, 0).r;
    //vec4 transparentColor = vec4(accum.rgb / clamp(accum.a, 6.1*1e-4, 6.55*1e5), r);
    //if (isinf(maxComponent(abs(accum))))
	//	accum.rgb = vec3(accum.a);
	vec3 transpColor = accum.rgb / max(accum.a, 0.00001);
	vec3 opaqueColor = transmissionColor.rgb * sampleLod(in_OpaqueColor, refractUV, transmissionColor.a).rgb;

	out_ColorBuffer0 = transpColor * (1 - coverage) + opaqueColor * coverage;
}
)""