/*
* @Author: gpinchon
* @Date:   2021-03-12 16:01:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:25
*/

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Light/DirectionalLight.hpp>
#include <Core/Inherit.hpp>
#include <Core/Property.hpp>
#include <SphericalHarmonics.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Lights {
class SkyLight : public Core::Inherit<DirectionalLight, SkyLight> {
    READONLYPROPERTY(float, SunPower, 20);
    READONLYPROPERTY(float, PlanetRadius, 6360e3);
    READONLYPROPERTY(float, AtmosphereRadius, 6420e3);
    READONLYPROPERTY(float, HRayleigh, 7994);
    READONLYPROPERTY(float, HMie, 1200);
    READONLYPROPERTY(glm::vec3, BetaRayleigh, 5.5e-6, 13.0e-6, 22.4e-6);
    READONLYPROPERTY(glm::vec3, BetaMie, 21e-6);

public:
    SkyLight();
    glm::vec3 GetSunDirection() const;
    void SetSunDirection(const glm::vec3& dir);
    void SetSunPower(const float power);
    /**
     * @arg radius : Planet's radius in meters
    */
    void SetPlanetRadius(float radius);
    /**
     * @arg radius : Planet's atmosphere radius in meters
    */
    void SetAtmosphereRadius(float radius);
    /**
     * @arg hRayleigh : Rayleigh scale heigh
    */
    void SetHRayleigh(float hRayleigh);
    /**
     * @arg hMie : Mie scale heigh
    */
    void SetHMie(float hMie);
    /**
     * @arg betaRayleigh : Rayleigh's scattering coefficient at sea level
    */
    void SetBetaRayleigh(glm::vec3 betaRayleigh);
    /**
     * @arg betaMie : Mie's scattering coefficient at sea level
    */
    void SetBetaMie(glm::vec3 betaMie);

    glm::vec3 GetIncidentLight(glm::vec3 direction) const;
};
}