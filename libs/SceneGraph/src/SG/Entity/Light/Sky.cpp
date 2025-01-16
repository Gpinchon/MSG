/*
 * @Author: gpinchon
 * @Date:   2021-03-12 16:08:58
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-05-18 18:26:25
 */
#include <SG/Component/Light/Sky.hpp>

#include <Tools/Pi.hpp>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

#include <cstdint>

namespace TabGraph::SG::Component {
uint32_t& GetNbr()
{
    static auto s_Nbr = 0u;
    return s_Nbr;
}

constexpr auto num_samples       = 16;
constexpr auto num_samples_light = 8;

/// \private
struct ray_t {
    glm::vec3 origin;
    glm::vec3 direction;
};

/// \private
struct sphere_t {
    glm::vec3 origin;
    float radius;
};

bool isect_sphere(const ray_t ray, const sphere_t sphere, float& t0, float& t1)
{
    glm::vec3 rc  = sphere.origin - ray.origin;
    float radius2 = sphere.radius * sphere.radius;
    float tca     = dot(rc, ray.direction);
    float d2      = dot(rc, rc) - tca * tca;
    if (d2 > radius2)
        return false;
    float thc = sqrt(radius2 - d2);
    t0        = tca - thc;
    t1        = tca + thc;

    return true;
}

float rayleigh_phase_func(float mu)
{
    return 3. * (1. + mu * mu)
        / //------------------------
        (16. * M_PI);
}

float henyey_greenstein_phase_func(float mu)
{
    const float g = 0.76;
    return (1. - g * g)
        / //---------------------------------------------
        ((4. * M_PI) * pow(1. + g * g - 2. * g * mu, 1.5));
}

bool get_sun_light(const ray_t& ray, float& optical_depthR, float& optical_depthM, const LightSky& sky)
{
    float t0, t1;
    const sphere_t atmosphere = sphere_t {
        glm::vec3(0, 0, 0), sky.atmosphereRadius
    };
    isect_sphere(ray, atmosphere, t0, t1);

    float march_pos  = 0.;
    float march_step = t1 / float(num_samples_light);

    for (int i = 0; i < num_samples_light; i++) {
        glm::vec3 s  = ray.origin + ray.direction * float(march_pos + 0.5 * march_step);
        float height = length(s) - sky.planetRadius;
        if (height < 0.)
            return false;

        optical_depthR += exp(-height / sky.hRayleigh) * march_step;
        optical_depthM += exp(-height / sky.hMie) * march_step;

        march_pos += march_step;
    }

    return true;
}

glm::vec3 GetSkyLight(const glm::vec3& a_Direction, const LightSky& sky)
{
    ray_t ray                 = { glm::vec3(0, sky.planetRadius + 1, 0), a_Direction };
    const sphere_t atmosphere = sphere_t {
        glm::vec3(0, 0, 0), sky.atmosphereRadius
    };
    // "pierce" the atmosphere with the viewing ray
    float t0, t1;
    if (!isect_sphere(
            ray, atmosphere, t0, t1)) {
        return glm::vec3(0);
    }

    float march_step = t1 / float(num_samples);

    // cosine of angle between view and light directions
    float mu = glm::dot(ray.direction, sky.sunDirection);

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

    glm::vec3 sumR  = glm::vec3(0);
    glm::vec3 sumM  = glm::vec3(0);
    float march_pos = 0.;

    for (int i = 0; i < num_samples; i++) {
        glm::vec3 s  = ray.origin + ray.direction * float(march_pos + 0.5 * march_step);
        float height = glm::length(s) - sky.planetRadius;

        // integrate the height scale
        float hr = exp(-height / sky.hRayleigh) * march_step;
        float hm = exp(-height / sky.hMie) * march_step;
        optical_depthR += hr;
        optical_depthM += hm;

        // gather the sunlight
        ray_t light_ray {
            s,
            sky.sunDirection
        };
        float optical_depth_lightR = 0.;
        float optical_depth_lightM = 0.;
        bool overground            = get_sun_light(
            light_ray,
            optical_depth_lightR,
            optical_depth_lightM,
            sky);

        if (overground) {
            glm::vec3 tau         = sky.betaRayleigh * (optical_depthR + optical_depth_lightR) + sky.betaMie * 1.1f * (optical_depthM + optical_depth_lightM);
            glm::vec3 attenuation = exp(-tau);

            sumR += hr * attenuation;
            sumM += hm * attenuation;
        }

        march_pos += march_step;
    }

    return sky.sunPower * (sumR * phaseR * sky.betaRayleigh + sumM * phaseM * sky.betaMie);
}
}
