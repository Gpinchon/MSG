R""(
struct t_Light {
	float   SpecularFactor;
    float   DiffuseFactor;
	vec3	Position;
	vec3	Color;
	float	Radius;
	float	Power;
#ifdef SHADOW
	mat4	Projection;
	sampler2DShadow	Shadow;
#endif //SHADOW
};

uniform t_Light			Light;

float LightAttenuation()
{
	float r = Light.Radius;
    vec3 L = Light.Position - WorldPosition();
    float D = length(L);
	return pow(max(1 - (D / r), 0), Light.Power);
    /*float d = max(distance - r, 0);
    L /= D;
     
    // calculate basic attenuation
    float denom = d/r + 1;
    float attenuation = 1 / (denom*denom);
     
    // scale and bias attenuation such that:
    //   attenuation == 0 at extent of max influence
    //   attenuation == 1 when d == 0
    attenuation = (attenuation - cutoff) / (1 - cutoff);
    return max(attenuation, 0);*/
}

float SpecularFactor(const in float NdotH, const in float SpecularPower)
{
	return ((SpecularPower + 2) / 8 ) * pow(clamp(NdotH, 0, 1), SpecularPower);
}

float GlossToSpecularPower(const in float gloss)
{
	return exp2(10 * gloss + 1);
}

void	Lighting()
{
	bvec3	isZero = equal(Light.Color, vec3(0));
	if (all(isZero)) {
		return ;
	}
	vec3	L = Light.Position - WorldPosition();
	float	D = length(L);
	L = normalize(L);
	vec3	V = normalize(Camera.Position - WorldPosition());
	vec3	N = WorldNormal();
	float constant = 1.0;
    float linear = 0.7;
    float quadratic = 1.8;
	//float	Attenuation = 1 / (constant + linear * D + quadratic * pow(D, 2)); //Light.Power * (1 / pow(distance(WorldPosition(), Light.Position), 2));
    float	Attenuation = min(1, LightAttenuation());
	vec3	H = normalize(V + L);
	float	NdotL = max(dot(N , L), 0.0);
	float	NdotH = max(dot(N , H), 0.0);
	vec3	Diffuse = Light.Color * NdotL;
	const vec3	Specular = Light.Color * SpecularFactor(NdotH, GlossToSpecularPower(1 - sqrt(Alpha())));
	SetDiffuse(vec4(Diffuse * Attenuation * Light.DiffuseFactor, 1));
    SetReflection(min(vec4(Specular * Attenuation * Light.SpecularFactor, 1), 10.f));
}
)""