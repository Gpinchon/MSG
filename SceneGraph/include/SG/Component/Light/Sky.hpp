/*
 * @Author: gpinchon
 * @Date:   2021-03-12 16:01:21
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-05-04 20:02:25
 */

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <glm/vec3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
struct LightSky {
    float sunPower { 20 };
    float planetRadius { 6360e3 }; // Planet's radius in meters
    float atmosphereRadius { 6420e3 }; // Planet's atmosphere radius in meters
    float hRayleigh { 7994 }; // Rayleigh scale heigh
    float hMie { 1200 }; // Mie scale heigh
    glm::vec3 betaRayleigh { 5.5e-6, 13.0e-6, 22.4e-6 }; // Rayleigh's scattering coefficient at sea level
    glm::vec3 betaMie { 21e-6 }; // Mie's scattering coefficient at sea level
    glm::vec3 sunDirection { 0, 1, 0 };
};
}
