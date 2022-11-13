R""(
#define M_PI	3.1415926535897932384626433832795
#define PI		M_PI
#define EPSILON	0.0001
#define lequal(a, b) all(lessThanEqual(a, b))
#define Luminance(linearColor) dot(linearColor, vec3(0.299, 0.587, 0.114))

//Pass
#define DeferredGeometry		0
#define DeferredLighting		1
#define DeferredMaterial		2
#define ForwardTransparent		3
#define ForwardOpaque			4
#define ShadowDepth				5
#define GeometryPostTreatment	6

struct t_Frag {
	vec3		WorldPosition;
	vec3		WorldNormal;
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

struct t_BRDF {
	vec3	CDiff;
	vec3	F0;
	float	Alpha;
};

#if Pass == DeferredMaterial
struct t_LightingTextures {
	sampler2D	Diffuse;
	sampler2D	Reflection;
};
#endif

struct t_GeometryTextures {
	sampler2D	CDiff;
	sampler2D	F0;
	sampler2D	Normal;
	sampler2D	Velocity;
	sampler2D	Depth;
};

in VertexOutput {
	vec2	TexCoord;
	vec3	CubeTexCoord;
} Input;

vec2 TexCoord() {
	return Input.TexCoord;
}

vec3 CubeTexCoord() {
	return Input.CubeTexCoord;
}

struct t_Textures {
	t_GeometryTextures Geometry;
#if Pass == DeferredMaterial
	t_LightingTextures Lighting;
#endif
};

#define map(value, low1, high1, low2, high2) (low2 + (value - low1) * (high2 - low2) / (high1 - low1))

uniform vec3			Resolution;
uniform float			Time;
uniform uint			FrameNumber;
uniform t_Camera		PrevCamera;
uniform t_Camera		Camera;
uniform t_Textures		Texture;

#if Pass == DeferredLighting
vec4	_CDiff; //BRDF CDiff, Ambient Occlusion
vec4	_F0; //BRDF F0, BRDF Alpha
vec3	_Normal;
vec3	_Velocity;
layout(location = 0) out vec4	_Diffuse;
layout(location = 1) out vec4	_Reflection;
#define _Emissive _Diffuse

#define Diffuse() (_Diffuse)
#define SetDiffuse(diffuse) (_Diffuse = diffuse)

#define Reflection() (_Reflection)
#define SetReflection(reflection) (_Reflection = reflection)

#elif Pass == DeferredMaterial
vec4	_CDiff; //BRDF CDiff, Ambient Occlusion
vec4	_F0; //BRDF F0, BRDF Alpha
vec3	_Normal;
vec3	_Velocity;
vec4	_Diffuse;
vec4	_Reflection;
layout(location = 0) out vec4 _Color;
#define _Emissive _Diffuse

#define Diffuse() (_Diffuse)
#define SetDiffuse(diffuse) (_Diffuse = diffuse)

#define Reflection() (_Reflection)
#define SetReflection(reflection) (_Reflection = reflection)

#elif Pass == GeometryPostTreatment
layout(location = 0) out vec4	_CDiff; //BRDF CDiff, Ambient Occlusion
layout(location = 1) out vec4	_F0; //BRDF F0, BRDF Alpha
layout(location = 2) out vec3	_Normal;
layout(location = 3) out vec3	_Velocity;
layout(location = 4) out vec4	_Color;
#define _Emissive _Color

#endif

#define _AO _CDiff.a
#define _Alpha _F0.a

#define CDiff() (_CDiff.rgb)
#define SetCDiff(cDiff) (_CDiff.rgb = cDiff)

#define F0() (_F0.rgb)
#define SetF0(f0) (_F0.rgb = f0)

#define Alpha() (_Alpha)
#define SetAlpha(alpha) (_Alpha = alpha)

#define AO() (_AO)
#define SetAO(ao) (_AO = ao)

#define WorldNormal() (_Normal)
#define SetWorldNormal(normal) (_Normal = normalize(normal))

#define Velocity() (_Velocity)
#define SetVelocity(velocity) (_Velocity = velocity)

#define Diffuse() (_Diffuse)
#define SetDiffuse(diffuse) (_Diffuse = diffuse)

#define Reflection() (_Reflection)
#define SetReflection(reflection) (_Reflection = reflection)

#define Emissive() (_Emissive)
#define SetEmissive(emissive) (_Emissive = emissive)

float _depth;
float Depth() { return _depth; }
float Depth(vec2 uv) { return textureLod(Texture.Geometry.Depth, uv, 0).r; }

#define ScreenTexCoord() (gl_FragCoord.xy / Resolution.xy)

t_Frag	Frag;

mat3x3	tbn_matrix()
{
	vec3 Q1 = dFdx(gl_FragCoord.xyz);
	vec3 Q2 = dFdy(gl_FragCoord.xyz);
	vec2 st1 = dFdx(TexCoord());
	vec2 st2 = dFdy(TexCoord());
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = normalize(-Q1*st2.s + Q2*st1.s);
	return(transpose(mat3(T, B, WorldNormal())));
}

vec4	WorldToView(in vec3 position)
{
	return Camera.Matrix.View * vec4(position, 1);
}

vec4	ViewToClip(in vec4 position)
{
	return Camera.Matrix.Projection * position;
}

vec4	ClipToScreen(in vec4 position)
{
	position.xyz /= position.w;
	position.xyz = position.xyz * 0.5 + 0.5;
	return position;
}

/** Returns the World position and the scene depth in world units */
vec4	WorldToScreen(in vec3 position)
{
	return ClipToScreen(ViewToClip(WorldToView(position)));
}

vec4	WorldToClip(in vec3 position)
{
	return ViewToClip(WorldToView(position));
}

vec3 TangentToWorld(in vec3 vec)
{
	return normalize(vec * tbn_matrix());
}

vec3	TangentToWorld(in vec2 vec)
{
	return TangentToWorld(vec3(vec, 1));
}

vec3	ScreenToWorld(in vec2 uv, in float depth)
{
	vec4	projectedCoord = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	projectedCoord = Camera.InvMatrix.View * Camera.InvMatrix.Projection * projectedCoord;
	return (projectedCoord.xyz / projectedCoord.w);
}

vec3	WorldPosition(in vec2 uv)
{
	return ScreenToWorld(uv, Depth(uv));
}

vec3 WorldPosition()
{
	return ScreenToWorld(ScreenTexCoord(), Depth());
}

/**
 * @brief Model -> World -> View -> Clip -> Screen
 * This does Clip <- Screen
 */
vec3 ClipPosition(const in vec2 uv)
{
	return vec3(uv, Depth(uv)) * 2.0 - 1.0;
}

vec3 ClipPosition()
{
	return ClipPosition(ScreenTexCoord());
}

/**
 * @brief Model -> World -> View -> Clip -> Screen
 * This does View <- Clip <- Screen
 */
vec3 ViewPosition(const in vec2 uv)
{
	vec4 clipPos = vec4(ClipPosition(uv), 1.0);
	vec4 viewPos = Camera.InvMatrix.Projection * clipPos;
	return (viewPos.xyz / viewPos.w);
}

vec3 ViewPosition()
{
	return ViewPosition(ScreenTexCoord());
}

#ifndef textureQueryLevels
float compMax(vec3 v) { return max(max(v.x, v.y), v.z); }
float compMax(vec2 v) { return max(v.x, v.y); }
#define textureQueryLevels(tex) int(log2(compMax(textureSize(tex, 0))))
#endif

#define sampleLod(tex, uv, lod) textureLod(tex, uv, lod * textureQueryLevels(tex))

vec4	texelFetchLod(in sampler2D tex, in vec2 uv, in float mipLevel)
{
	ivec2	Resolution = textureSize(tex, 0);
	return mix(texelFetch(tex, ivec2(Resolution * uv), int(floor(mipLevel))), texelFetch(tex, ivec2(Resolution * uv), int(ceil(mipLevel))), fract(mipLevel));
}

vec4	texelFetchLod(in sampler2D tex, in vec2 uv, in int mipLevel)
{
	return texelFetch(tex, ivec2(textureSize(tex, 0) * uv), mipLevel);
}

float	smootherstep(float edge0, float edge1, float x) {
	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	return x * x * x * (x * (x * 6 - 15) + 10);
}

bool	isZero(in float v)
{
	return (abs(v) < EPSILON);
}

bool	isZero(in vec2 v)
{
	return all(equal(v, vec2(0)));
}

void FillFragmentData(void)
{
	vec4	CDiff_sample = textureLod(Texture.Geometry.CDiff, ScreenTexCoord(), 0);
	vec4	F0_sample = textureLod(Texture.Geometry.F0, ScreenTexCoord(), 0);
	vec4	Normal_sample = textureLod(Texture.Geometry.Normal, ScreenTexCoord(), 0);
	vec4	Velocity_sample = textureLod(Texture.Geometry.Velocity, ScreenTexCoord(), 0);
	vec4	Depth_sample = textureLod(Texture.Geometry.Depth, ScreenTexCoord(), 0);
#if Pass == DeferredMaterial
	vec4	Diffuse_sample = textureLod(Texture.Lighting.Diffuse, ScreenTexCoord(), 0);
	vec4	Reflection_sample = textureLod(Texture.Lighting.Reflection, ScreenTexCoord(), 0);
	_Diffuse = Diffuse_sample.rgba;
	_Reflection = Reflection_sample.rgba;
#endif
	_CDiff = CDiff_sample.rgba; _AO = CDiff_sample.a;
	_F0 = F0_sample.rgba; _Alpha = F0_sample.a;
	_Normal = Normal_sample.rgb;
	_Velocity = Velocity_sample.rgb;
	_depth = Depth_sample.r;
	WorldPosition(ScreenTexCoord());
}
)""