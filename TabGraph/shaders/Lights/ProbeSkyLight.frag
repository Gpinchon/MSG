R""(
layout(location = 0) out vec4   out_0;

#define PI 3.14159265359
#define NUMSAMPLES 16
#define NUMSAMPLESLIGHT 8

uniform float DiffuseFactor = 1;
uniform vec3  SunDirection = vec3(0, 1, 0);
uniform float SunPower = 20;
uniform float PlanetRadius = 6360e3;
uniform float AtmosphereRadius = 6420e3;
uniform float HRayleigh = 7994;
uniform float HMie = 1200;
uniform vec3  BetaRayleigh = vec3(5.5e-6, 13.0e-6, 22.4e-6);
uniform vec3  BetaMie = vec3(21e-6);
uniform vec3  Resolution;

struct ray_t {
    vec3 origin;
    vec3 direction;
};

struct sphere_t {
    vec3 origin;
    float radius;
};

bool isect_sphere(const in ray_t ray, const in sphere_t sphere, inout float t0, inout float t1)
{
    vec3 rc = sphere.origin - ray.origin;
    float radius2 = sphere.radius * sphere.radius;
    float tca = dot(rc, ray.direction);
    float d2 = dot(rc, rc) - tca * tca;
    if (d2 > radius2) return false;
    float thc = sqrt(radius2 - d2);
    t0 = tca - thc;
    t1 = tca + thc;

    return true;
}

float rayleigh_phase_func(float mu)
{
    return
            3. * (1. + mu*mu)
    / //------------------------
                (16. * PI);
}

// Henyey-Greenstein phase function factor [-1, 1]
// represents the average cosine of the scattered directions
// 0 is isotropic scattering
// > 1 is forward scattering, < 1 is backwards
const float g = 0.76;
float henyey_greenstein_phase_func(float mu)
{
    return
                        (1. - g*g)
    / //---------------------------------------------
        ((4. * PI) * pow(1. + g*g - 2.*g*mu, 1.5));
}

const sphere_t atmosphere = sphere_t (
    vec3(0, 0, 0), AtmosphereRadius
);

bool get_sun_light(
    const in ray_t ray,
    inout float optical_depthR,
    inout float optical_depthM
){
    float t0, t1;
    isect_sphere(ray, atmosphere, t0, t1);

    float march_pos = 0.;
    float march_step = t1 / float(NUMSAMPLESLIGHT);

    for (int i = 0; i < NUMSAMPLESLIGHT; i++) {
        vec3 s =
            ray.origin +
            ray.direction * (march_pos + 0.5 * march_step);
        float height = length(s) - PlanetRadius;
        if (height < 0.)
            return false;

        optical_depthR += exp(-height / HRayleigh) * march_step;
        optical_depthM += exp(-height / HMie) * march_step;

        march_pos += march_step;
    }

    return true;
}

vec3 get_incident_light(const in ray_t ray)
{
    // "pierce" the atmosphere with the viewing ray
    float t0, t1;
    if (!isect_sphere(
        ray, atmosphere, t0, t1)) {
        return vec3(0);
    }

    float march_step = t1 / float(NUMSAMPLES);

    // cosine of angle between view and light directions
    float mu = dot(ray.direction, SunDirection);

    // Rayleigh and Mie phase functions
    // A black box indicating how light is interacting with the material
    // Similar to BRDF except
    // * it usually considers a single angle
    //   (the phase angle between 2 directions)
    // * integrates to 1 over the entire sphere of directions
    float phaseR = rayleigh_phase_func(mu);
    float phaseM = henyey_greenstein_phase_func(mu);

    // optical depth (or "average density")
    // represents the accumulated extinction coefficients
    // along the path, multiplied by the length of that path
    float optical_depthR = 0.;
    float optical_depthM = 0.;

    vec3 sumR = vec3(0);
    vec3 sumM = vec3(0);
    float march_pos = 0.;

    for (int i = 0; i < NUMSAMPLES; i++) {
        vec3 s =
            ray.origin +
            ray.direction * (march_pos + 0.5 * march_step);
        float height = length(s) - PlanetRadius;

        // integrate the height scale
        float hr = exp(-height / HRayleigh) * march_step;
        float hm = exp(-height / HMie) * march_step;
        optical_depthR += hr;
        optical_depthM += hm;

        // gather the sunlight
        ray_t light_ray = ray_t (
            s,
            SunDirection
        );
        float optical_depth_lightR = 0.;
        float optical_depth_lightM = 0.;
        bool overground = get_sun_light(
            light_ray,
            optical_depth_lightR,
            optical_depth_lightM);

        if (overground) {
            vec3 tau =
                BetaRayleigh * (optical_depthR + optical_depth_lightR) +
                BetaMie * 1.1 * (optical_depthM + optical_depth_lightM);
            vec3 attenuation = exp(-tau);

            sumR += hr * attenuation;
            sumM += hm * attenuation;
        }

        march_pos += march_step;
    }

    return
        SunPower *
        (sumR * phaseR * BetaRayleigh +
         sumM * phaseM * BetaMie);
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

vec2 ScreenTexCoord() {
    return gl_FragCoord.xy / Resolution.xy;
}

void ComputeSkyLight() {
    const vec3  Direction = normalize(CubeMapUVToXYZ(gl_Layer, ScreenTexCoord()));
    out_0.rgb = get_incident_light(ray_t(
            vec3(0, PlanetRadius + 1, 0), Direction
        )) * DiffuseFactor;
}

)""