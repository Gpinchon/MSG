#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Component.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward Declaration
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
class Texture;
}

////////////////////////////////////////////////////////////////////////////////
// Class Declaration
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
class ParticleSystem : public Component {
public:
    /** @brief the size of each particle in world unit */
    float size = 1.f;

    /** @brief is this particle system rendered to the shadow map ? */
    bool castShadow = false;
    /** @brief life expetancy of each particle in seconds */
    float lifeExpetancy = 1.f;
    /** @brief number of particles to start with */
    uint32_t startingCount = 0;
    struct {
        /** @brief how opaque the particles are */
        float opacity = 1;
        /** @brief how emissive the aprticles are */
        float emissivity = 0;
        /** @brief the albedo of the particles in the absence of a MaterialSet */
        glm::vec3 albedoFactor = glm::vec3(1);
        /** @brief the emissive factor of the particles in the absence of a MaterialSet */
        glm::vec3 emissiveFactor = glm::vec3(1);
        std::shared_ptr<Texture> albedoTexture;
        std::shared_ptr<Texture> emissiveTexture;
    } material;
    struct {
        bool enabled = false;
        float bounce = 0.5f;
    } collision;
    struct {
        /** @brief is this particle system emitting particles ? */
        bool enabled = true;
        /** @brief the number of particles emitted every seconds */
        float rate = 100.f;
        /** @brief the seed used to generate the particle system */
        uint32_t seed = 0xFFFFFFFF;
    } emission;
};
}
