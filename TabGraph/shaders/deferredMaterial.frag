R""(
uniform sampler2D BRDFLUT;

#define BRDF(NdV, Roughness) (texture(BRDFLUT, vec2(NdV, Roughness)).xy)

void CalculateLighting() {
	if (Depth() == 1)
		discard;
	vec3	diffuseSample = Diffuse().rgb;
	vec3	reflectionSample = Reflection().rgb;
	vec3	V = normalize(Camera.Position - WorldPosition());
	vec3	N = WorldNormal();
	float	NdV = max(dot(N, V), 0);
	//vec3	R = reflect(V, N);
	float	alphaSqrt = sqrt(Alpha());
	//float	sampleLOD = min(alphaSqrt * 2.f, 1.f);
	vec2	brdf = BRDF(NdV, alphaSqrt);
	vec3	fresnel = F0() * brdf.x + brdf.y;
	vec3	diffuse = diffuseSample * CDiff();
	vec3	specular = reflectionSample * fresnel;
	_Color = vec4(diffuse + specular, 1.f/* min(1, Opacity() + Luminance(specular)) */);
	//_Diffuse.rgb = fromLinear(_Diffuse.rgb);
}
)""