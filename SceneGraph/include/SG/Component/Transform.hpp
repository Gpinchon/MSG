#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Common.hpp>
#include <SG/Core/Property.hpp>

#include <glm/ext/quaternion_float.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
class Transform {
    PROPERTY(glm::vec3, Up, Common::Up());
    PROPERTY(glm::vec3, Right, Common::Right());
    PROPERTY(glm::vec3, Forward, Common::Forward());
    READONLYPROPERTY(glm::vec3, Position, 0);
    READONLYPROPERTY(glm::vec3, Scale, 1);
    READONLYPROPERTY(glm::quat, Rotation, glm::vec3(0.0, 0.0, 0.0));
    READONLYPROPERTY(glm::mat4, TranslationMatrix, 1);
    READONLYPROPERTY(glm::mat4, ScaleMatrix, 1);
    READONLYPROPERTY(glm::mat4, RotationMatrix, 1);
    READONLYPROPERTY(glm::mat4, TransformMatrix, 1);

public:
    /** @brief sets the position & updates the translation matrix */
    void SetPosition(const glm::vec3& a_Position)
    {
        if (a_Position == GetPosition())
            return;
        _SetPosition(a_Position);
        _SetTranslationMatrix(glm::translate(glm::mat4(1), GetPosition()));
        _UpdateTransformMatrix();
    }
    /** @brief sets the scale & updates the scale matrix */
    void SetScale(const glm::vec3& a_Scale)
    {
        if (a_Scale == GetScale())
            return;
        _SetScale(a_Scale);
        _SetScaleMatrix(glm::scale(glm::mat4(1), GetScale()));
        _UpdateTransformMatrix();
    }
    /** @brief sets the rotation & updates the rotation matrix */
    void SetRotation(const glm::quat& a_Rotation)
    {
        if (a_Rotation == GetRotation())
            return;
        _SetRotation(a_Rotation);
        _SetRotationMatrix(glm::mat4_cast(GetRotation()));
        _UpdateTransformMatrix();
    }

private:
    void _UpdateTransformMatrix()
    {
        _SetTransformMatrix(GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix());
    }
};

// creating an "alias" to differentiate components
class WorldTransform : public Transform {
    using Transform::Transform;
};
}
