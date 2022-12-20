#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/gtc/quaternion.hpp>

#include <SG/Common.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
struct Transform {
    /** @brief sets the position & updates the translation matrix */
    void SetPosition(const glm::vec3& a_Position) {
        position = a_Position;
        translationMatrix = glm::translate(glm::mat4(1), position);
    }
    /** @brief sets the scale & updates the scale matrix */
    void SetScale(const glm::vec3& a_Scale) {
        scale = a_Scale;
        scaleMatrix = glm::scale(glm::mat4(1), a_Scale);
    }
    /** @brief sets the rotation & updates the rotation matrix */
    void SetRotation(const glm::quat& a_Rotation) {
        rotation = a_Rotation;
        rotationMatrix = glm::mat4_cast(rotation);
    }

    glm::vec3 position{ 0 };
    glm::vec3 scale{ 0 };
    glm::quat rotation{ glm::vec3(0.0, 0.0, 0.0) };
    
    glm::mat4 translationMatrix{ 1 };
    glm::mat4 scaleMatrix{ 1 };
    glm::mat4 rotationMatrix{ 1 };

    glm::vec3 forward{ Common::Forward() };
    glm::vec3 right{ Common::Forward() };
    glm::vec3 up{ Common::Forward() };
};
}