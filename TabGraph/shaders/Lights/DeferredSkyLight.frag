R""(
struct t_Light {
    float   SpecularFactor;
    float   DiffuseFactor;
    vec3    Min;
    vec3    Max;
    bool    Infinite;
#ifdef SHADOW
    mat4    Projection;
    sampler2DShadow Shadow;
#endif //SHADOW
};

uniform t_Light Light;
uniform samplerCube SpecularLUT;

void    Lighting()
{
	const vec3  V = normalize(WorldPosition() - Camera.Position);
    const vec3  N = WorldNormal();
    const vec3  R = reflect(V, N);
    const float alphaSqrt = sqrt(Alpha());
    vec3        Diffuse = vec3(SampleSH(N)) * Light.DiffuseFactor;
    vec3        Specular = sampleLod(SpecularLUT, R, alphaSqrt * 2).rgb * Light.SpecularFactor;
    float       Attenuation = 1;
    #ifdef SHADOW
        float bias = 0.01 * tan(acos(NdotL));
        Attenuation *= SampleShadowMap(clamp(bias, 0.0f, 0.02f), Light.Projection, Light.Shadow);
        if (Attenuation == 0)
            return ;
        Diffuse *= Attenuation;
        Specular * Attenuation
    #endif //SHADOW
    bool isAboveMax = any(greaterThan(WorldPosition(), Light.Max));
    bool isUnderMin = any(lessThan(WorldPosition(), Light.Min));
    if (!Light.Infinite && (isAboveMax || isUnderMin)) {
        return ;
    }
    SetDiffuse(vec4(Diffuse, 1));
    SetReflection(vec4(Specular, 1));
}
)""