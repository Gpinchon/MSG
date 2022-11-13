R""(
#define PI      3.1415926535897932384626433832795

struct t_Light {
    vec3    Direction;
    vec3    Color;
};

layout(location = 0) out vec4   out_0;
uniform t_Light         Light;
uniform vec3            Resolution;

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
    const vec3  L = normalize(Light.Direction);
    const vec3  N = normalize(CubeMapUVToXYZ(gl_Layer, ScreenTexCoord()));
    const float NdotL = max(0, -dot(N , L));
    out_0.rgb = Light.Color * SpecularFactor(NdotL, GlossToSpecularPower(1));
}
)""