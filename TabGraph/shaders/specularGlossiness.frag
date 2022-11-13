R""(

#ifdef SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS
#undef SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULAR
#undef SPECULAR_GLOSSINESS_TEXTURE_USE_GLOSSINESS
#endif

#if defined(SPECULAR_GLOSSINESS_TEXTURE_USE_DIFFUSE) || defined(SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS) || defined(SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULAR) || defined(SPECULAR_GLOSSINESS_TEXTURE_USE_GLOSSINESS)
#define SPECULAR_GLOSSINESS_USE_TEXTURES
#endif
//#ifdef TEXTURE_USE_SPECULAR
//uniform sampler2D	TextureSpecular;
//#endif
#ifdef SPECULAR_GLOSSINESS_USE_TEXTURES
struct t_SpecularGlossinessTextures {
	#ifdef SPECULAR_GLOSSINESS_TEXTURE_USE_DIFFUSE
	sampler2D	Diffuse;
	#endif
	#ifdef SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS
	sampler2D	SpecularGlossiness;
	#endif
	#ifdef SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULAR
	sampler2D	Specular;
	#endif
};
#endif

struct t_SpecularGlossinessValues {
	vec3	Diffuse;
	vec3	Specular;
	float	Glossiness;
	float	Opacity;
};

uniform vec3	SpecularGlossinessDiffuse = vec3(1);
uniform vec3	SpecularGlossinessSpecular = vec3(1);
uniform float	SpecularGlossinessGlossiness = float(1);
uniform float	SpecularGlossinessOpacity = float(1);
#ifdef SPECULAR_GLOSSINESS_USE_TEXTURES
uniform t_SpecularGlossinessTextures	SpecularGlossinessTextures;
#endif


void SpecularGlossiness() {
	t_SpecularGlossinessValues values;
	values.Diffuse = SpecularGlossinessDiffuse;
	values.Specular = SpecularGlossinessSpecular;
	values.Glossiness = SpecularGlossinessGlossiness;
	values.Opacity = SpecularGlossinessOpacity;
#ifdef SPECULAR_GLOSSINESS_TEXTURE_USE_DIFFUSE
	vec4 diffuseSample = texture(SpecularGlossinessTextures.Diffuse, TexCoord());
	values.Diffuse *= diffuseSample.rgb;
	values.Opacity *= diffuseSample.a;
#endif
#ifdef SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULAR
	values.Specular *= texture(SpecularGlossinessTextures.Specular, TexCoord()).rgb;
#endif
#ifdef SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS
	vec4	SpecularGlossiness_sample = texture(SpecularGlossinessTextures.SpecularGlossiness, TexCoord());
	values.Specular *= SpecularGlossiness_sample.xyz;
	values.Glossiness *= SpecularGlossiness_sample.w;
#endif
	SetCDiff(CDiff() * values.Diffuse.rgb * (1 - max(values.Specular.r, max(values.Specular.g, values.Specular.b))));
	SetF0(values.Specular);
	SetAlpha(Alpha() * max(0.05, pow(1 - values.Glossiness, 2)));
	SetOpacity(Opacity() * values.Opacity);
	//SetCDiff(mix(values.Diffuse.rgb * (1 - F0()), vec3(0), values.Glossiness));
	//SetF0(mix(F0(), values.Diffuse.rgb, values.Glossiness));
	//SetAlpha(Alpha() * map(values.Roughness * values.Roughness, 0, 1, 0.05, 1));
}

)""