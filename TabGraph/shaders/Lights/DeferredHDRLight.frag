R""(
struct t_Light {
    float   SpecularFactor;
    float   DiffuseFactor;
    vec3    Min;
    vec3    Max;
    bool    Infinite;
};

uniform t_Light Light;
uniform samplerCube ReflectionMap;

void    Lighting()
{
	const vec3  V = normalize(WorldPosition() - Camera.Position);
    const vec3  N = WorldNormal();
    const vec3  R = reflect(-V, N);
    const float alphaSqrt = sqrt(Alpha());
    vec3        Diffuse = vec3(SampleSH(N));
    vec3        Reflection = sampleLod(ReflectionMap, R, alphaSqrt * 2).rgb * Light.SpecularFactor;
    bool isAboveMax = any(greaterThan(WorldPosition(), Light.Max));
    bool isUnderMin = any(lessThan(WorldPosition(), Light.Min));
    if (!Light.Infinite && (isAboveMax || isUnderMin)) {
        return ;
    }
    if (length(N) == 0)
        discard;
    SetDiffuse(vec4(Diffuse * (1 - AO()) * Light.DiffuseFactor, 1));
    SetReflection(vec4(Reflection * Light.SpecularFactor, 1));
}
)""