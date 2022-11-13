R""(
float InterleavedGradientNoise(vec2 uv, float FrameId)
{
	// magic values are found by experimentation
	uv += FrameId * (vec2(47, 17) * 0.695f);

    const vec3 magic = vec3( 0.06711056f, 0.00583715f, 52.9829189f );
    return fract(magic.z * fract(dot(uv, magic.xy)));
}

#define NUM_OCTAVES 5

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

float fbm(vec3 x, float FrameId) {
    x += FrameId * (vec3(47, 17, 0) * 0.695f);
	float v = 0.0;
	float a = 0.5;
	vec3 shift = vec3(100);
	for (int i = 0; i < NUM_OCTAVES; ++i) {
		v += a * noise(x);
		x = x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}

//ivec4[4] stipple = ivec4[4](
//	ivec4(10,  0, 15,  5),
//	ivec4(12,  6,  1, 11),
//	ivec4( 2,  8,  7, 13),
//	ivec4( 4, 14,  9,  3)
//);
//
//bool StipplePattern(in vec2 coord) {
//	int noise = int(InterleavedGradientNoise(coord, FrameNumber % 8) * 16);
//	int Offset = int(Depth() * 15);
//	int Range = int(Opacity() * 15);
//	int Max = clamp(Offset + Range, 0, 15);
//	int Min = Max - Range;
//	int X = int(coord.x + noise) % 4;
//	int Y = int(coord.y + noise) % 4;
//	return	stipple[X][Y] >= Min &&
//			stipple[X][Y] <= Max;
//}

float hash(const in vec2 co) {
	return fract( 1.0e4 * sin( 17.0*co.x + 0.1*co.y ) *
	( 0.1 + abs( sin( 13.0*co.y + co.x )))
	);
}

float hash(const in vec3 co) {
	return hash( vec2( hash( co.xy ), co.z ) );
}

const float g_HashScale = sqrt(1.f / 1024.f);

float AlphaThreshold() {
	vec3 objCoord = WorldPosition().xyz;
	// Find the discretized derivatives of our coordinates
	vec3 anisoDeriv = max(
		abs(dFdx(objCoord.xyz)),
		abs(dFdy(objCoord.xyz))
	);
	vec3 anisoScales = vec3(
		0.707/(g_HashScale*anisoDeriv.x),
		0.707/(g_HashScale*anisoDeriv.y),
		0.707/(g_HashScale*anisoDeriv.z)
	);
	// Find log-discretized noise scales
	vec3 scaleFlr = vec3(
		exp2(floor(log2(anisoScales.x))),
		exp2(floor(log2(anisoScales.y))),
		exp2(floor(log2(anisoScales.z)))
	);
	vec3 scaleCeil = vec3(
		exp2(ceil(log2(anisoScales.x))),
		exp2(ceil(log2(anisoScales.y))),
		exp2(ceil(log2(anisoScales.z)))
	);
	// Compute alpha thresholds at our two noise scales
	vec2 alpha = vec2(hash(floor(scaleFlr*objCoord.xyz)),
	hash(floor(scaleCeil*objCoord.xyz)));
	// Factor to linearly interpolate with
	vec3 fractLoc = vec3(
		fract(log2(anisoScales.x)),
		fract(log2(anisoScales.y)),
		fract(log2(anisoScales.z))
	);
	vec2 toCorners = vec2( length(fractLoc),
	length(vec3(1.0f)-fractLoc) );
	float lerpFactor = toCorners.x/(toCorners.x+toCorners.y);
	// Interpolate alpha threshold from noise at two scales
	float x = (1-lerpFactor)*alpha.x + lerpFactor*alpha.y;
	// Pass into CDF to compute uniformly distrib threshold
	float a = min( lerpFactor, 1-lerpFactor );
	vec3 cases = vec3(
		x*x/(2*a*(1-a)),
		(x-0.5*a)/(1-a),
		1.0-((1-x)*(1-x)/(2*a*(1-a)))
	);
	// Find our final, uniformly distributed alpha threshold
	float noise = (x < (1-a)) ?
	((x < a) ? cases.x : cases.y) :
	cases.z;
	// Avoids noise == 0. Could also do noise = 1 - noise
	noise = clamp( noise , 1.0e-6, 1.0 );
	return noise;
}

bool StipplePattern(in vec2 coord) {
	float alphaThreshold = AlphaThreshold();
	//float alphaThreshold = InterleavedGradientNoise(coord, FrameNumber % 8);
	return Opacity() > alphaThreshold;
}

void	CheckOpacity()
{
	#if OpacityMode == Opaque
		SetOpacity(1);
		return;
	#endif
	#if OpacityMode == Mask
		if(Opacity() > OpacityCutoff()) {
			SetOpacity(1);
			return;
		}
		else
			discard;
	#endif
	#if OpacityMode == Blend && (Pass == DeferredGeometry || Pass == ForwardTransparent)
	if (Opacity() < 1.f)
		discard;
	#elif OpacityMode == Blend && Pass == ShadowDepth
	if (Opacity() < 1.f && Opacity() < InterleavedGradientNoise(gl_FragCoord.xy, FrameNumber % 8))
		discard;
	#endif
	
}
)""