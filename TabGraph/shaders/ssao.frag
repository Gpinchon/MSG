R""(
#ifndef SSAO_STRENGTH
#define SSAO_STRENGTH 1.f
#endif

#ifndef SSAO_STRENGTH
#define SSAO_RADIUS 0.05f
#endif

#if SSAO_QUALITY == 1
#define SAMPLENBR 4
#elif SSAO_QUALITY == 2
#define SAMPLENBR 8
#elif SSAO_QUALITY == 3
#define SAMPLENBR 16
#else //SSAO_QUALITY == 4
#define SAMPLENBR 32
#endif

float InterleavedGradientNoise(vec2 uv, float FrameId)
{
	// magic values are found by experimentation
	uv += FrameId * (vec2(47, 17) * 0.695f);

    const vec3 magic = vec3( 0.06711056f, 0.00583715f, 52.9829189f );
    return fract(magic.z * fract(dot(uv, magic.xy)));
}

vec2 Hammersley(uint SampleIdx, uint SampleCnt)
{
    float u = float(SampleIdx) / float(SampleCnt);
    float v = float(bitfieldReverse(SampleIdx)) * 2.3283064365386963e-10;
    return vec2(u, v);
}

void	SSAO()
{
	if (Depth() == 1)
		discard;
	float occlusion = 0.f;
	const vec3 N = WorldNormal();
	const vec3 P = WorldPosition();
    const uint frameIndex = FrameNumber % 8;
	const float noise = InterleavedGradientNoise(gl_FragCoord.xy, frameIndex) * 2 - 1;
	for (int i = 0; i < SAMPLENBR; ++i)
	{
		vec2 E = Hammersley(i, SAMPLENBR) * vec2(M_PI, 2 * M_PI);
		E.y += noise;
		vec2 sE= vec2(cos(E.y), sin(E.y)) * SSAO_RADIUS * cos(E.x);
		vec2 screenCoords = ScreenTexCoord() + sE;
		vec3 V = WorldPosition(screenCoords) - P;
        float d = length(V);
        V /= d;
        d *= SSAO_RADIUS;
		occlusion += max(0.0, dot(N, V) - 0.025) * (1.0 / (1.0 + d));
	}
	occlusion /= float(SAMPLENBR);
	occlusion *= SSAO_STRENGTH;
	SetAO(max(0, occlusion));
}

)""