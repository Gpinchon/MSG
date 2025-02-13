#include <MSG/Transform/Data.hpp>

bool MSG::TransformData::SetPosition(const glm::vec3& a_Position)
{
    if (a_Position == GetPosition())
        return false;
    _SetPosition(a_Position);
    _SetTranslationMatrix(glm::translate(glm::mat4(1), GetPosition()));
    _UpdateTransformMatrix();
    return true;
}

bool MSG::TransformData::SetScale(const glm::vec3& a_Scale)
{
    if (a_Scale == GetScale())
        return false;
    _SetScale(a_Scale);
    _SetScaleMatrix(glm::scale(glm::mat4(1), GetScale()));
    _UpdateTransformMatrix();
    return true;
}

bool MSG::TransformData::SetRotation(const glm::quat& a_Rotation)
{
    if (a_Rotation == GetRotation())
        return false;
    _SetRotation(a_Rotation);
    _SetRotationMatrix(glm::mat4_cast(GetRotation()));
    _UpdateTransformMatrix();
    return true;
}

void MSG::TransformData::_UpdateTransformMatrix()
{
    _SetTransformMatrix(GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix());
}
