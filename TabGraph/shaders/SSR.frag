R""(
layout(early_fragment_tests) in;
uniform sampler2D	LastColor;

#pragma optionNV (unroll all)

#if SSR_QUALITY == 1
#define NumSteps 8
#define NumRays 1
#elif SSR_QUALITY == 2
#define NumSteps 8
#define NumRays 2
#elif SSR_QUALITY == 3
#define NumSteps 8
#define NumRays 4
#else // SSR_QUALITY == 4
#define NumSteps 8
#define NumRays 8
#endif

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float alpha)
{	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (alpha*alpha - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

vec4	LastWorldToView(in vec3 position)
{
	return PrevCamera.Matrix.View * vec4(position, 1);
}

vec4	LastViewToClip(in vec4 position)
{
	return PrevCamera.Matrix.Projection * position;
}

/** Returns the World position and the scene depth in world units */
vec4	LastWorldToScreen(in vec3 position)
{
	return ClipToScreen(LastViewToClip(LastWorldToView(position)));
}

vec3	GetLastUVz(in vec3 UVz)
{
	vec3 worldPos = ScreenToWorld(UVz.xy, UVz.z);
	UVz = LastWorldToScreen(worldPos).xyz;
	return UVz;
}

vec3	GetLastUVz(in vec2 UV, in float z)
{
	return GetLastUVz(vec3(UV, z));
}

vec4 SampleDepthTexture(float Level, vec4 SampleUV0, vec4 SampleUV1 )
{
	vec4 SampleDepth;
	SampleDepth.x = textureLod(Texture.Geometry.Depth, SampleUV0.xy, Level).r;
	SampleDepth.y = textureLod(Texture.Geometry.Depth, SampleUV0.zw, Level).r;
	SampleDepth.z = textureLod(Texture.Geometry.Depth, SampleUV1.xy, Level).r;
	SampleDepth.w = textureLod(Texture.Geometry.Depth, SampleUV1.zw, Level).r;
	return SampleDepth;
}

//Simplified AABB-Ray Intersection
// See : https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
void IntersectClippingBounds(const in vec3 rayOrigin, const in vec3 rayDir, out vec3 rayEnd) {
	const vec3 vmin = vec3(-1, -1, -1);
	const vec3 vmax = vec3( 1,  1,  1);
	const vec3 t1 = (vmin - rayOrigin)/ rayDir;
    const vec3 t2 = (vmax - rayOrigin)/ rayDir;
    float tmin = min(t1.x, t2.x);
    float tmax = max(t1.x, t2.x);
    tmin = max(tmin, min(t1.y, t2.y));
    tmax = min(tmax, max(t1.y, t2.y));
    tmin = max(tmin, min(t1.z, t2.z));
    tmax = min(tmax, max(t1.z, t2.z));
	rayEnd = rayOrigin + rayDir * max(tmax, tmin);
}

const float Step = 1.f / float(NumSteps + 1);

vec4	castRay(const in vec3 RayStartClip, const in vec3 RayDirClip,  const in float StepOffset)
{
	vec3 RayEndClip;
	//Clip ray agains Normalized Device Coordinate's bounding box
	IntersectClippingBounds(RayStartClip, RayDirClip, RayEndClip);
	const vec3 RayStartUVz = RayStartClip * 0.5 + 0.5;
	const vec3 RayEndUVz = RayEndClip * 0.5 + 0.5;
	const vec3 RayStepUVz = (RayEndUVz - RayStartUVz);
	const float CompareTolerance = abs(RayStepUVz.z) * Step * 2;
	float LastDiff = 0;
	float Level = 0;
	float SampleTime = StepOffset * Step + Step;
	for (int i = 0; i < NumSteps; ++i) {
		const vec3 UVz = RayStartUVz + RayStepUVz * SampleTime;
		const float SampleZ = textureLod(Texture.Geometry.Depth, UVz.xy, Level).r;
		const float DepthDiff = SampleZ - UVz.z;
		if (abs(-DepthDiff - CompareTolerance) < CompareTolerance)
		{
			if (i > 0) { //else, this is the first loop, no need to go back
				// Find more accurate hit using line segment intersection
				float TimeLerp = clamp(LastDiff / (LastDiff - DepthDiff), 0, 1);
				SampleTime += TimeLerp * Step - Step;
			}
			return vec4(RayStartUVz + RayStepUVz * SampleTime, SampleTime * SampleTime);
		}
		LastDiff = DepthDiff;
		Level += (4.0 / NumSteps) * Alpha();
		SampleTime += Step;
	}
	/*vec4 SampleTime = (StepOffset * Step + Step) * vec4( 1, 2, 3, 4 );
	for (int i = 0; i < NumSteps; i += 4) {
		vec4 SampleUV0 = RayStartUVz.xyxy + RayStepUVz.xyxy * SampleTime.xxyy;
		vec4 SampleUV1 = RayStartUVz.xyxy + RayStepUVz.xyxy * SampleTime.zzww;
		vec4 SampleZ   = RayStartUVz.zzzz + RayStepUVz.zzzz * SampleTime;
		vec4 SampleDepth = SampleDepthTexture(Level, SampleUV0, SampleUV1);
		vec4 DepthDiff = SampleDepth - SampleZ;
		bvec4 Hit = lessThan(abs(-DepthDiff - CompareTolerance), vec4(CompareTolerance));
		//bvec4 Hit = lessThan(SampleDepth, SampleZ);
		if (any(Hit))
		{
			float DepthDiff0 = DepthDiff[2];
            float DepthDiff1 = DepthDiff[3];
            float Time0 = 3;
            if (Hit[2])
            {
                DepthDiff0 = DepthDiff[1];
                DepthDiff1 = DepthDiff[2];
                Time0 = 2;
            }
            if (Hit[1])
            {
                DepthDiff0 = DepthDiff[0];
                DepthDiff1 = DepthDiff[1];
                Time0 = 1;
            }
            if (Hit[0])
            {
                DepthDiff0 = LastDiff;
                DepthDiff1 = DepthDiff[0];
                Time0 = 0;
            }
			Time0 += float(i);
			float IntersectTime = Time0 * Step;
			if (Time0 > 1) {
				float TimeLerp = clamp(DepthDiff0 / (DepthDiff0 - DepthDiff1), 0, 1);
				IntersectTime += TimeLerp * Step - Step;
			}
            return vec4(RayStartUVz + RayStepUVz * IntersectTime, IntersectTime * IntersectTime);
		}
		LastDiff = DepthDiff.w;
		Level += Alpha() * (16.0 * Step);
		SampleTime += 4.0 * Step;
	}*/
	return vec4(0, 0, 0, 1);
}

vec4	SampleScreenColor(vec3 UVz)
{
	UVz = GetLastUVz(UVz);
	return texture(LastColor, UVz.xy, 0);
}

float GetRoughnessFade()
{
	// mask SSR to reduce noise and for better performance, roughness of 0 should have SSR, at MaxRoughness we fade to 0
	return min(sqrt(Alpha()) * ROUGHNESSMASKSCALE + 2, 1.0);
}

void	SSR()
{
	_Reflection = vec4(0);
	const float	RoughnessFade = GetRoughnessFade();
	const vec3	Position = WorldPosition();
	const vec3	Normal = WorldNormal();
	const vec3	V = normalize(Position - Camera.Position);
	const uint	frameIndex = FrameNumber % 8;
	const uvec3	Random = Rand3DPCG16(ivec3(gl_FragCoord.xy, frameIndex));
	const float	StepOffset = InterleavedGradientNoise(gl_FragCoord.xy, frameIndex) - 0.5;
	const mat4	TransformMatrix = Camera.Matrix.Projection * Camera.Matrix.View;
	vec3	RayStartClip = vec3(ScreenTexCoord(), Depth()) * 2 - 1;
	const int	RayNbr = Alpha() < 0.05 ? 1 : NumRays;
	if (RayNbr > 1) {
		for(int i = 0; i < NumRays; i++ ) {
			//Generate random normals using Hammersley distribution
			const vec2	E = Hammersley16(i, NumRays, Random.xy);
			//Compute Half vector using GGX Importance Sampling
			//Project Half vector from Tangent space to World space
			vec3	H = ImportanceSampleGGX(E, Normal, Alpha());
			vec3	R = normalize(reflect(V, H));
/*
			vec4	RayDirClip = TransformMatrix * vec4(R, 1);
			RayDirClip.xyz /= abs(RayDirClip.w);
			RayDirClip.xyz = normalize(RayDirClip.xyz);
*/

			vec4	RayEndClip	= TransformMatrix * vec4(Position + R * 1000, 1);
			RayEndClip.xyz /= abs(RayEndClip.w);
			vec3	RayDirClip = normalize(RayEndClip.xyz - RayStartClip.xyz);

			vec4	SSRResult = castRay(RayStartClip.xyz, RayDirClip.xyz, StepOffset);
			float	SSRParticipation = clamp(4 - 4 * SSRResult.w, 0, 1);
			//Attenuate reflection factor when getting closer to screen border
			vec4	SampleColor = vec4(SampleScreenColor(SSRResult.xyz).rgb, SSRParticipation);
			SampleColor.rgb /= 1 + Luminance(SampleColor.rgb);
			_Reflection += SampleColor * SSRParticipation;
		}
		_Reflection /= float(RayNbr);
	}
	else {
		vec3	R = reflect(V, WorldNormal());
		vec4	RayEndClip	= TransformMatrix * vec4(Position + R * 1000, 1);
		RayEndClip.xyz /= abs(RayEndClip.w);
		vec3	RayDirClip = normalize(RayEndClip.xyz - RayStartClip.xyz);
		vec4	SSRResult = castRay(RayStartClip.xyz, RayDirClip, StepOffset);
		float	SSRParticipation = clamp(4 - 4 * SSRResult.w, 0, 1);
		//Attenuate reflection factor when getting closer to screen border
		vec4	SampleColor = vec4(SampleScreenColor(SSRResult.xyz).rgb, SSRParticipation);
		SampleColor.rgb /= 1 + Luminance(SampleColor.rgb);
		_Reflection = SampleColor * SSRParticipation;
	}
	_Reflection.rgb /= 1 - Luminance(_Reflection.rgb);
	_Reflection *= RoughnessFade;
}

)""