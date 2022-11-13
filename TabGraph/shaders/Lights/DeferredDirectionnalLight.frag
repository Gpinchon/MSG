R""(
struct t_Light {
    float   SpecularFactor;
    float   DiffuseFactor;
    vec3    Min;
    vec3    Max;
    vec3    Direction;
    vec3    Color;
    bool    Infinite;
#ifdef SHADOW
    mat4    Projection;
    sampler2DShadow Shadow;
#endif //SHADOW
};

uniform t_Light     Light;

float SpecularFactor(const in float NdotH, const in float SpecularPower)
{
    return ((SpecularPower + 2) / 8 ) * pow(clamp(NdotH, 0, 1), SpecularPower);
}

float GlossToSpecularPower(const in float gloss)
{
    return exp2(10 * gloss + 1);
}

void    Lighting()
{
    const vec3  V = -normalize(WorldPosition() - Camera.Position);
    const vec3  N = WorldNormal();
    const vec3  L = Light.Direction;
    const vec3  H = normalize(L + V);
    const float NdV = max(0, dot(N, V));
    const float NdL = max(0, dot(N, L));
    const float NdH = max(0, dot(N, H));
    //const float alphaSqrt = sqrt(Alpha());
    //const vec2    brdf = texture(DefaultBRDFLUT, vec2(NdV, alphaSqrt)).xy;
    //const vec3    fresnel = F0() * brdf.x + brdf.y;
    
    vec3        Diffuse = Light.Color * NdL;
    vec3        Specular = Diffuse * SpecularFactor(NdH, GlossToSpecularPower(1 - sqrt(Alpha())));
    float       Attenuation = 1;
    #ifdef SHADOW
        float bias = 0.01 * tan(acos(NdL));
        Attenuation *= SampleShadowMap(clamp(bias, 0.0f, 0.02f), Light.Projection, Light.Shadow);
        if (Attenuation == 0)
            return ;
        Diffuse *= Attenuation;
        Specular *= Attenuation;
    #endif //SHADOW
    if (all(equal(Diffuse, vec3(0)))) {
        return ;
    }
    bool isAboveMax = any(greaterThan(WorldPosition(), Light.Max));
    bool isUnderMin = any(lessThan(WorldPosition(), Light.Min));
    if (!Light.Infinite && (isAboveMax || isUnderMin)) {
        return ;
    }
    SetDiffuse(vec4(Diffuse * Light.DiffuseFactor, 1));
    SetReflection(vec4(Specular * Light.SpecularFactor, 1));
}

)""