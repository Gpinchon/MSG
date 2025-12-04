#include <MSG/Transform.hpp>

void Msg::Transform::LookAt(const glm::vec3& a_Target)
{
    auto direction  = glm::normalize(a_Target - GetWorldPosition());
    auto directionL = glm::length(direction);
    auto up         = GetLocalUp();
    if (!(directionL > 0.0001)) {
        SetLocalRotation(glm::quat(1, 0, 0, 0));
        return;
    }
    direction /= directionL;
    if (glm::abs(glm::dot(direction, up)) > 0.9999f) {
        up = glm::vec3(1, 0, 0);
    }
    SetLocalRotation(glm::quatLookAt(direction, up));
}

void Msg::Transform::LookAt(const Transform& a_Target)
{
    return LookAt(a_Target.GetWorldPosition());
}

void Msg::Transform::UpdateWorld(const Transform& a_Parent)
{
    if (!_worldNeedsUpdate)
        return;
    const auto posMat = glm::translate(a_Parent._world.GetTransformMatrix(), _local.GetPosition());
    const auto sclMat = glm::scale(a_Parent._world.GetScaleMatrix(), _local.GetScale());
    const auto rotMat = a_Parent._world.GetRotationMatrix() * _local.GetRotationMatrix();
    const auto pos    = posMat * glm::vec4(0, 0, 0, 1);
    const auto scl    = sclMat * glm::vec4(1, 1, 1, 0);
    const auto rot    = glm::quat_cast(rotMat);
    _world.SetPosition(pos);
    _world.SetScale(scl);
    _world.SetRotation(rot);
    _world.SetUp(rotMat * glm::vec4(_local.GetUp(), 0));
    _world.SetRight(rotMat * glm::vec4(_local.GetRight(), 0));
    _world.SetForward(rotMat * glm::vec4(_local.GetForward(), 0));
    _worldNeedsUpdate = false;
}

void Msg::Transform::UpdateWorld()
{
    return UpdateWorld({});
}
