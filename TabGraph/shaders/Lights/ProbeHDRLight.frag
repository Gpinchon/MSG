R""(
struct t_Light {
    vec3    Min;
    vec3    Max;
    bool    Infinite;
};

uniform t_Light Light;
uniform samplerCube ReflectionMap;
uniform vec3        Resolution;
uniform vec3		ProbePosition;
layout(location = 0) out vec4   out_0;

vec2 ScreenTexCoord() {
    return gl_FragCoord.xy / Resolution.xy;
}

vec3 CubeMapUVToXYZ(const int index, vec2 uv)
{
    vec3 xyz = vec3(0);
    // convert range 0 to 1 to -1 to 1
    uv = uv * 2.f - 1.f;
    switch (index)
    {
    case 0:
        xyz = vec3(1.0f, -uv.y, -uv.x);
        break;  // POSITIVE X
    case 1:
        xyz = vec3(-1.0f, -uv.y, uv.x);
        break;  // NEGATIVE X
    case 2:
        xyz = vec3(uv.x, 1.0f, uv.y);
        break;  // POSITIVE Y
    case 3:
        xyz = vec3(uv.x, -1.0f, -uv.y);
        break;  // NEGATIVE Y
    case 4:
        xyz = vec3(uv.x, -uv.y, 1.0f);
        break;  // POSITIVE Z
    case 5:
        xyz = vec3(-uv.x, -uv.y, -1.0f);
        break;  // NEGATIVE Z
    }
    return normalize(xyz);
}

void    Lighting()
{
	const vec3  N = normalize(CubeMapUVToXYZ(gl_Layer, ScreenTexCoord()));
	const vec3  Reflection = texture(ReflectionMap, N, 0).rgb;
	const bool isAboveMax = any(greaterThan(ProbePosition, Light.Max));
    const bool isUnderMin = any(lessThan(ProbePosition, Light.Min));
    if (!Light.Infinite && (isAboveMax || isUnderMin)) {
        return ;
    }
	out_0 = vec4(Reflection, 1);
}
)""