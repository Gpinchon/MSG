R""(
in float CameraSpaceDepth;
uniform sampler2D	BRDFLUT;
uniform samplerCube	ReflectionMap;

#define Fresnel(factor, f0, alpha) (f0 + (max(f0, 1 - alpha) - f0) * pow(1 - max(factor, 0), 5))
#define BRDF(NdV, Roughness) (texture(BRDFLUT, vec2(NdV, Roughness)).xy)

#if Pass == ForwardTransparent
float LinearDepth(const in float z)
{
	float	depth = z;
	vec2	uv = TexCoord();
	vec4	projectedCoord = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	projectedCoord = Camera.InvMatrix.Projection * projectedCoord;
	float A = Camera.Matrix.Projection[2].z;
    float B = Camera.Matrix.Projection[3].z;
	float zNear = - B / (1.0 - A);
    float zFar  =   B / (1.0 + A);
	zFar = isinf(zFar) ? 1000.f : zFar;
	return (-projectedCoord.z / projectedCoord.w + zNear) / (zFar - zNear);
}

float LinearDepth()
{
	float z = abs(CameraSpaceDepth);
	float A = Camera.Matrix.Projection[2].z;
    float B = Camera.Matrix.Projection[3].z;
	float zNear = - B / (1.0 - A);
    float zFar  =   B / (1.0 + A);
	return 0.5*(-A*z + B) / z + 0.5;
}

void WritePixel(vec3 premultipliedColor, float coverage)
{
	float z = LinearDepth();
	float a = min(1.0, coverage) * 8.0 + 0.01;
    float b = -gl_FragCoord.z * 0.95 + 1.0;
    float w = clamp(a * a * a * 1e8 * b * b * b, 1e-2, 3e2);
	//float w = clamp(pow(z, -4), 6.1*1e-4, 1e5);

	//float w = clamp(pow(abs(1 / z), 4.f) * coverage * coverage, 6.1*1e-4, 1e5);

	SetColor(vec4(premultipliedColor, coverage) * w);
	SetAlphaCoverage(coverage);
}
#endif

void ComputeRefraction(in vec3 N, const in vec3 V) {
	if (dot(N, V) < 0)
		N = -N;
	vec4	viewNormal = Camera.Matrix.View * vec4(N, 0);
	vec3	viewV = normalize(-vec3(0, 0, distance(WorldPosition(), Camera.Position)));
	vec4	refractDir = vec4(refract(viewV, viewNormal.xyz, 1.f/ Ior()), 0);
	vec2	refractOffset = refractDir.xy * 0.05;// * min(0.05, depthDiff);//min(0.025, depthDiff);
	SetDistortion(refractOffset);
}

void ComputeTransmission() {
	SetTransmissionColor(mix(vec3(1), CDiff(), Opacity()));
	SetTransmissionRoughness(Alpha() * Opacity());
}

void ComputeColor() {
#if Pass == ForwardTransparent
	//float opaqueDepth = texture(OpaqueDepthTexture, ScreenTexCoord(), 0).r;
	#ifndef FASTTRANSPARENCY
	//if (opaqueDepth < gl_FragCoord.z)
	//	discard;
	#endif
#endif
	const float alphaSqrt = sqrt(Alpha());
	const vec3 V = normalize(Camera.Position - WorldPosition());
	const vec3 N = normalize(WorldNormal());
	const vec3 R = reflect(V, N);
	const float NdV = max(0, dot(N, V));
	const vec2 brdf = BRDF(NdV, alphaSqrt);
	const vec3 fresnel = F0() * brdf.x + brdf.y;
	const vec3 diffuse = (CDiff() * VertexDiffuse) + (CDiff() * BackVertexDiffuse * (1 - Opacity()));
	const vec3 specular = fresnel * sampleLod(ReflectionMap, R, alphaSqrt * 2).rgb;
	const vec3 color = specular + diffuse + Emissive();
#if Pass == ForwardTransparent
	if (Opacity() == 1 || Opacity() <= 0.005) discard;
	WritePixel(color * Opacity(), Opacity());
	ComputeRefraction(N, V);
	ComputeTransmission();
#elif Pass == ForwardOpaque
	SetColor(color);
	#if OpacityMode == Opaque
		SetOpacity(1);
	#endif
	#if OpacityMode == Mask
		if(Opacity() > OpacityCutoff())
			SetOpacity(1);
		discard;
	#endif
#endif
}

)""