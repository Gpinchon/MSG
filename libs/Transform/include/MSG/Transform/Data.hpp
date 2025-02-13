#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Core/Orientation.hpp>
#include <MSG/Core/Property.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class TransformData {
    PROPERTY(glm::vec3, Up, Core::Up());
    PROPERTY(glm::vec3, Right, Core::Right());
    PROPERTY(glm::vec3, Forward, Core::Forward());
    READONLYPROPERTY(glm::vec3, Position, 0);
    READONLYPROPERTY(glm::vec3, Scale, 1);
    READONLYPROPERTY(glm::quat, Rotation, glm::vec3(0.0, 0.0, 0.0));
    READONLYPROPERTY(glm::mat4, TranslationMatrix, 1);
    READONLYPROPERTY(glm::mat4, ScaleMatrix, 1);
    READONLYPROPERTY(glm::mat4, RotationMatrix, 1);
    READONLYPROPERTY(glm::mat4, TransformMatrix, 1);

public:
    /** @brief sets the position & updates the translation matrix */
    bool SetPosition(const glm::vec3& a_Position);
    /** @brief sets the scale & updates the scale matrix */
    bool SetScale(const glm::vec3& a_Scale);
    /** @brief sets the rotation & updates the rotation matrix */
    bool SetRotation(const glm::quat& a_Rotation);

private:
    void _UpdateTransformMatrix();
};
}
