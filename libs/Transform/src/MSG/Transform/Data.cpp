#include <MSG/Transform/Data.hpp>

bool Msg::TransformData::SetUp(const glm::vec3& a_Up)
{
    if (a_Up == GetUp())
        return false;
    _SetUp(a_Up);
    return true;
}

bool Msg::TransformData::SetRight(const glm::vec3& a_Right)
{
    if (a_Right == GetRight())
        return false;
    _SetRight(a_Right);
    return true;
}

bool Msg::TransformData::SetForward(const glm::vec3& a_Forward)
{
    if (a_Forward == GetForward())
        return false;
    _SetForward(a_Forward);
    return true;
}

bool Msg::TransformData::SetPosition(const glm::vec3& a_Position)
{
    if (a_Position == GetPosition())
        return false;
    _SetPosition(a_Position);
    _SetTranslationMatrix(glm::translate(glm::mat4(1), GetPosition()));
    _UpdateTransformMatrix();
    return true;
}

bool Msg::TransformData::SetScale(const glm::vec3& a_Scale)
{
    if (a_Scale == GetScale())
        return false;
    _SetScale(a_Scale);
    _SetScaleMatrix(glm::scale(glm::mat4(1), GetScale()));
    _UpdateTransformMatrix();
    return true;
}

bool Msg::TransformData::SetRotation(const glm::quat& a_Rotation)
{
    if (a_Rotation == GetRotation())
        return false;
    _SetRotation(a_Rotation);
    _SetRotationMatrix(glm::mat4_cast(GetRotation()));
    _UpdateTransformMatrix();
    return true;
}

void Msg::TransformData::_UpdateTransformMatrix()
{
    _SetTransformMatrix(GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix());
}
