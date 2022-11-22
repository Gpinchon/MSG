/*
* @Author: gpinchon
* @Date:   2021-03-12 16:01:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:25
*/

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Light/Light.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Property.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class LightSky : public Inherit<Light, LightSky> {
    PROPERTY(float, SunPower, 20);
    PROPERTY(float, PlanetRadius, 6360e3);      //Planet's radius in meters
    PROPERTY(float, AtmosphereRadius, 6420e3);  //Planet's atmosphere radius in meters
    PROPERTY(float, HRayleigh, 7994);           //Rayleigh scale heigh
    PROPERTY(float, HMie, 1200);                //Mie scale heigh
    PROPERTY(glm::vec3, BetaRayleigh, 5.5e-6, 13.0e-6, 22.4e-6);    //Rayleigh's scattering coefficient at sea level
    PROPERTY(glm::vec3, BetaMie, 21e-6);                            //Mie's scattering coefficient at sea level
    PROPERTY(glm::vec3, SunDirection, 0, 1, 0);

public:
    LightSky();
    LightSky(const std::string& a_Name) : LightSky() {
        SetName(a_Name);
    }
    glm::vec3 GetIncidentLight(glm::vec3 direction) const;
};
}