////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Core/Orientation.hpp>
#include <MSG/Core/Property.hpp>

#include <glm/ext/quaternion_float.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

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

inline bool MSG::TransformData::SetPosition(const glm::vec3& a_Position)
{
    if (a_Position == GetPosition())
        return false;
    _SetPosition(a_Position);
    _SetTranslationMatrix(glm::translate(glm::mat4(1), GetPosition()));
    _UpdateTransformMatrix();
    return true;
}

inline bool MSG::TransformData::SetScale(const glm::vec3& a_Scale)
{
    if (a_Scale == GetScale())
        return false;
    _SetScale(a_Scale);
    _SetScaleMatrix(glm::scale(glm::mat4(1), GetScale()));
    _UpdateTransformMatrix();
    return true;
}

inline bool MSG::TransformData::SetRotation(const glm::quat& a_Rotation)
{
    if (a_Rotation == GetRotation())
        return false;
    _SetRotation(a_Rotation);
    _SetRotationMatrix(glm::mat4_cast(GetRotation()));
    _UpdateTransformMatrix();
    return true;
}

inline void MSG::TransformData::_UpdateTransformMatrix()
{
    _SetTransformMatrix(GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix());
}
