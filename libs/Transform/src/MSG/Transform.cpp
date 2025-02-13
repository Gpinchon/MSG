#pragma once

#include <MSG/Transform.hpp>

void MSG::Transform::UpdateWorld(const Transform& a_Parent)
{
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
