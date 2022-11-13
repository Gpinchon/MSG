R""(
#define M_PI 3.1415926535897932384626433832795
#define Luminance(linearColor) dot(linearColor, vec3(0.299, 0.587, 0.114))

#ifndef textureQueryLevels
float compMax(vec3 v) { return max(max(v.x, v.y), v.z); }
float compMax(vec2 v) { return max(v.x, v.y); }
#define textureQueryLevels(tex) int(log2(compMax(textureSize(tex, 0))))
#endif

#define sampleLod(tex, uv, lod) textureLod(tex, uv, lod * textureQueryLevels(tex))

struct t_Matrix {
	mat4	Model;
	mat3	Normal;
};

struct t_Environment {
	samplerCube	Diffuse;
	samplerCube	Irradiance;
};

//Pass
#define DeferredGeometry		0
#define DeferredLighting		1
#define DeferredMaterial		2
#define ForwardTransparent		3
#define ForwardOpaque			4
#define ShadowDepth				5
#define GeometryPostTreatment	6

//OpacityMode
#define Opaque	0
#define Mask	1
#define Blend	2

struct t_BRDF {
	vec3	CDiff;
	vec3	F0;
	float	Alpha;
};

struct t_Frag {
	float		Depth;
	vec2		TexCoord;
	vec3		WorldPosition;
};

struct t_CameraMatrix {
	mat4	View;
	mat4	Projection;
};

struct t_Camera {
	vec3			Position;
	t_CameraMatrix	Matrix;
	t_CameraMatrix	InvMatrix;
};

uniform t_Camera			Camera;
uniform t_Camera			PrevCamera;
#ifdef TEXTURE_USE_HEIGHT
uniform sampler2D			StandardTextureHeight;
#endif
#ifdef TEXTURE_USE_DIFFUSE
uniform sampler2D			StandardTextureDiffuse;
#endif
#ifdef TEXTURE_USE_EMISSIVE
uniform sampler2D			StandardTextureEmissive;
#endif
#ifdef TEXTURE_USE_NORMAL
uniform sampler2D			StandardTextureNormal;
#endif
#ifdef TEXTURE_USE_AO
uniform sampler2D			StandardTextureAO;
#endif

uniform vec3				StandardDiffuse = vec3(1);
uniform vec3				StandardEmissive = vec3(0);
uniform float				StandardOpacityCutoff = float(0.5);
uniform float				StandardOpacity = float(1);
uniform float				StandardParallax = float(0.05);
uniform float				StandardIor = float(1);
uniform float				StandardAO = float(0);
uniform t_Matrix			Matrix;
uniform samplerCube			Skybox;
uniform vec3				Resolution;
uniform float				Time;
uniform uint				FrameNumber;

in VertexOutput {
	vec3	WorldPosition;
	vec3	WorldNormal;
	vec2	TexCoord;
} Input;

in vec3 ModelPosition;
in vec4 Position;
in vec4 PreviousPosition;
#if Pass == ForwardTransparent || Pass == ForwardOpaque
in vec3 VertexDiffuse;
#if OpacityMode == Blend
in vec3 BackVertexDiffuse;
#endif //OpacityMode == Blend
#endif

#if Pass == DeferredGeometry
layout(location = 0) out vec4	_CDiff; //BRDF CDiff, Ambient Occlusion
layout(location = 1) out vec4	_F0; //BRDF F0, BRDF Alpha
layout(location = 2) out vec3	_Normal;
layout(location = 3) out vec2	_Velocity;
layout(location = 4) out vec4	_Color;
#define _Emissive _Color.rgb
#endif //Pass == DeferredGeometry

#if Pass == ForwardTransparent || Pass == ForwardOpaque || Pass == ShadowDepth
vec4	_CDiff = vec4(0); //BRDF CDiff, Ambient Occlusion
vec4	_F0 = vec4(0); //BRDF F0, BRDF Alpha
vec3	_Normal = vec3(0);
vec2	_Velocity = vec2(0);
vec3	_Emissive = vec3(0);
#endif //Pass == ForwardTransparent || Pass == ForwardOpaque || Pass == ShadowDepth

#if Pass == ForwardTransparent
layout(location = 0) out vec4	_Color;
layout(location = 1) out float	_AlphaCoverage;
layout(location = 2) out vec2	_Distortion;
layout(location = 3) out vec4	_Transmission;

#define Color() (_Color)
#define AlphaCoverage() (_AlphaCoverage)
#define Distortion() (_Distortion)
#define TransmissionColor() (_Transmission.rgb)
#define TransmissionRoughness() (_Transmission.a)

#define SetColor(color) (_Color = color)
#define SetAlphaCoverage(alphaCoverage) (_AlphaCoverage = alphaCoverage)
#define SetDistortion(distortion) (_Distortion = distortion)
#define SetTransmissionColor(color) (_Transmission.rgb = color)
#define SetTransmissionRoughness(roughness) (_Transmission.a = roughness)
#endif //Pass == ForwardTransparent

#if Pass == ForwardOpaque
layout(location = 0) out vec4	_Color;
#define Color() (_Color)
#define SetColor(color) (_Color = color)
#endif //Pass == ForwardOpaque

float _Opacity = 1.f;

#define _AO _CDiff.a
#define _Alpha _F0.a

#define ScreenTexCoord() (gl_FragCoord.xy / Resolution.xy)

#define OpacityCutoff() (StandardOpacityCutoff)

#define Opacity() (_Opacity)
#define SetOpacity(opacity) (_Opacity = opacity)

#define CDiff() (_CDiff.rgb)
#define SetCDiff(cDiff) (_CDiff.rgb = cDiff)

#define F0() (_F0.rgb)
#define SetF0(f0) (_F0.rgb = f0)

#define Alpha() (_Alpha)
#define SetAlpha(alpha) (_Alpha = alpha)

#define Emissive() (_Emissive)
#define SetEmissive(emissive) (_Emissive = emissive)

#define Ior() (StandardIor)

#define AO() (_AO)
#define SetAO(aO) (_AO = aO)

t_Frag	Frag;

#define map(value, low1, high1, low2, high2) (low2 + (value - low1) * (high2 - low2) / (high1 - low1))

bool _WorldPositionSet = false;
bool _WorldNormalSet = false;
bool _TexCoordSet = false;
bool _DepthSet = false;

void SetWorldPosition(in vec3 worldPosition)
{
	Frag.WorldPosition = worldPosition;
	_WorldPositionSet = true;
}

vec3 WorldPosition()
{
	if (!_WorldPositionSet)
		SetWorldPosition(Input.WorldPosition);
	return Frag.WorldPosition;
}

void SetWorldNormal(in vec3 worldNormal)
{
	_Normal = normalize(worldNormal);
	_WorldNormalSet = true;
}

vec3 WorldNormal()
{
	if (!_WorldNormalSet)
		SetWorldNormal(Input.WorldNormal);
	return _Normal;
}

void SetTexCoord(vec2 texCoord)
{
	Frag.TexCoord = texCoord;
	_TexCoordSet = true;
}

vec2 TexCoord()
{
	if (!_TexCoordSet)
		SetTexCoord(Input.TexCoord);
	return Frag.TexCoord;
}

void SetDepth(in float depth)
{
	Frag.Depth = depth;
#ifdef FORCEDEPTHWRITE
	gl_FragDepth = depth;
#endif //FORCEDEPTHWRITE
	_DepthSet = true;
}

float Depth()
{
	if (!_DepthSet)
		SetDepth(gl_FragCoord.z);
	return Frag.Depth;
}

#define Parallax() (StandardParallax)

#ifdef TEXTURE_USE_HEIGHT
void	Parallax_Mapping(in vec3 tbnV, out float parallaxHeight)
{
	vec2 T = TexCoord();
	const float minLayers = 10;
	const float maxLayers = 15;
	float numLayers = mix(maxLayers, minLayers, abs(tbnV.z));
	int	tries = int(numLayers);
	float layerHeight = 1.0 / numLayers;
	float curLayerHeight = 0;
	vec2 dtex = Parallax() * tbnV.xy / tbnV.z / numLayers;
	vec2 currentTextureCoords = T;
	float heightFromTexture = 1 - texture(StandardTextureHeight, currentTextureCoords).r;
	while(tries > 0 && heightFromTexture > curLayerHeight) 
	{
		tries--;
		curLayerHeight += layerHeight; 
		currentTextureCoords -= dtex;
		heightFromTexture = 1 - texture(StandardTextureHeight, currentTextureCoords).r;
	}
	vec2 prevTCoords = currentTextureCoords + dtex;
	float nextH	= heightFromTexture - curLayerHeight;
	float prevH	= 1 - texture(StandardTextureHeight, prevTCoords).r
	- curLayerHeight + layerHeight;
	float weight = nextH / (nextH - prevH);
	vec2 finalTexCoords = prevTCoords * weight + currentTextureCoords * (1.0 - weight);
	parallaxHeight = (curLayerHeight + prevH * weight + nextH * (1.0 - weight));
	parallaxHeight *= Parallax();
	parallaxHeight = isnan(parallaxHeight) ? 0 : parallaxHeight;
	SetTexCoord(finalTexCoords);
}
#endif

mat3x3	tbn_matrix()
{
	vec3 Q1 = dFdx(WorldPosition());
	vec3 Q2 = dFdy(WorldPosition());
	vec2 st1 = dFdx(TexCoord());
	vec2 st2 = dFdy(TexCoord());
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = normalize(-Q1*st2.s + Q2*st1.s);
	return(transpose(mat3(T, B, WorldNormal())));
}

void	FillFragmentData()
{
	vec3 a = Position.xyz / Position.w * 0.5 + 0.5;
	vec3 b = PreviousPosition.xyz / PreviousPosition.w * 0.5 + 0.5;
	_Velocity = b.xy - a.xy;
	SetF0(vec3(0.04f));
	SetAlpha(1.f);
	SetWorldNormal(Input.WorldNormal);
	SetDepth(gl_FragCoord.z);
#if defined(TEXTURE_USE_HEIGHT) || defined(TEXTURE_USE_NORMAL)
	mat3	tbn = tbn_matrix();
#endif
#ifdef TEXTURE_USE_HEIGHT
	float ph = 0;
	vec3 viewDir = normalize(Camera.Position - WorldPosition());
	Parallax_Mapping(tbn * viewDir, ph);
#endif
#ifdef TEXTURE_USE_DIFFUSE
	vec4	albedo_sample = texture(StandardTextureDiffuse, TexCoord());
	SetCDiff(StandardDiffuse * albedo_sample.rgb);
	SetOpacity(StandardOpacity * albedo_sample.a);
#else
	SetCDiff(StandardDiffuse);
	SetOpacity(StandardOpacity);
#endif
#ifdef TEXTURE_USE_EMISSIVE
	vec3 emissive = texture(StandardTextureEmissive, TexCoord()).rgb;
	SetEmissive(StandardEmissive * emissive);
#else
	SetEmissive(StandardEmissive);
#endif
#ifdef TEXTURE_USE_AO
	SetAO(StandardAO + max(1 - texture(StandardTextureAO, TexCoord()).r, 0));
#else
	SetAO(StandardAO);
#endif
#ifdef TEXTURE_USE_NORMAL
	vec3	normal_sample = texture(StandardTextureNormal, TexCoord()).xyz * 2 - 1;
	vec3	new_normal = normal_sample * tbn;
	SetWorldNormal(normalize(new_normal));
#endif
#ifdef TEXTURE_USE_HEIGHT
	SetWorldPosition(WorldPosition() - (WorldNormal() * ph));
#endif
}

)""