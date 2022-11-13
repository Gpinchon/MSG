R""(

#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS
#undef METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS
#undef METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC
#endif

#if defined(METALLIC_ROUGHNESS_TEXTURE_USE_BASECOLOR) || defined(METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS) || defined(METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC) || defined(METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS)
#define METALLIC_ROUGHNESS_USE_TEXTURES
#endif

#ifdef METALLIC_ROUGHNESS_USE_TEXTURES
struct t_MetallicRoughnessTextures {
	#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_BASECOLOR
	sampler2D	BaseColor;
	#endif
	#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS
	sampler2D	Roughness;
	#endif
	#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC
	sampler2D	Metallic;
	#endif
	#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS
	sampler2D	MetallicRoughness;
	#endif
};
#endif

struct t_MetallicRoughnessValues {
	vec3	BaseColor;
	float	Roughness;
	float	Metallic;
	float	Opacity;
};

uniform vec3	MetallicRoughnessBaseColor = vec3(1);
uniform float	MetallicRoughnessRoughness = float(1);
uniform float	MetallicRoughnessMetallic = float(1);
uniform float	MetallicRoughnessOpacity = float(1);

#ifdef METALLIC_ROUGHNESS_USE_TEXTURES
uniform t_MetallicRoughnessTextures	MetallicRoughnessTextures;
#endif


void MetallicRoughness() {
	t_MetallicRoughnessValues values;
	values.BaseColor = MetallicRoughnessBaseColor;
	values.Roughness = MetallicRoughnessRoughness;
	values.Metallic = MetallicRoughnessMetallic;
	values.Opacity = MetallicRoughnessOpacity;
#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_BASECOLOR
	vec4 baseColor = texture(MetallicRoughnessTextures.BaseColor, TexCoord());
	values.BaseColor *= baseColor.rgb;
	values.Opacity *= baseColor.a;
#endif
#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS
	values.Roughness *= texture(MetallicRoughnessTextures.Roughness, TexCoord()).r;
#endif
#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC
	values.Metallic *= texture(MetallicRoughnessTextures.Metallic, TexCoord()).r;
#endif
#ifdef METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS
	vec2	metallicRoughness_sample = texture(MetallicRoughnessTextures.MetallicRoughness, TexCoord()).gb;
	values.Roughness *= metallicRoughness_sample.x;
	values.Metallic *= metallicRoughness_sample.y;
#endif
	SetCDiff(CDiff() * mix(values.BaseColor.rgb * (1. - 0.04), vec3(0), values.Metallic));
	SetF0(mix(vec3(0.04), values.BaseColor.rgb , values.Metallic));
	SetAlpha(max(0.05, values.Roughness * values.Roughness));
	SetOpacity(Opacity() * values.Opacity);
}

)""