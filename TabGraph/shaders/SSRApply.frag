R""(
uniform sampler2D	SSRTexture;

void SSRApply()
{
	const vec3  V = normalize(Camera.Position - WorldPosition());
    const vec3  N = WorldNormal();
    const float NdV = max(0, dot(N, V));
	const float alphaSqrt = sqrt(Alpha());
	const vec4	SSR = texture(SSRTexture, ScreenTexCoord());
	_Diffuse = vec4(0);
	_Reflection = vec4(SSR);
}
)""