#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Common.hpp>
#include <SG/Core/Property.hpp>
#include <Tools/Debug.hpp>

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
class TransformData {
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
    bool SetPosition(const glm::vec3& a_Position)
    {
        if (a_Position == GetPosition())
            return false;
        _SetPosition(a_Position);
        _SetTranslationMatrix(glm::translate(glm::mat4(1), GetPosition()));
        _UpdateTransformMatrix();
        return true;
    }
    /** @brief sets the scale & updates the scale matrix */
    bool SetScale(const glm::vec3& a_Scale)
    {
        if (a_Scale == GetScale())
            return false;
        _SetScale(a_Scale);
        _SetScaleMatrix(glm::scale(glm::mat4(1), GetScale()));
        _UpdateTransformMatrix();
        return true;
    }
    /** @brief sets the rotation & updates the rotation matrix */
    bool SetRotation(const glm::quat& a_Rotation)
    {
        if (a_Rotation == GetRotation())
            return false;
        _SetRotation(a_Rotation);
        _SetRotationMatrix(glm::mat4_cast(GetRotation()));
        _UpdateTransformMatrix();
        return true;
    }

private:
    void _UpdateTransformMatrix()
    {
        _SetTransformMatrix(GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix());
    }
};

#define CHECK_UPDATE                                \
    {                                               \
        if (_worldNeedsUpdate) {                    \
            debugLog("World transform outdated !"); \
            throw std::runtime_error("");           \
        }                                           \
    }
class Transform {
public:
    const auto& GetLocal() const { return _local; }
    auto& GetLocalUp() const { return _local.GetUp(); }
    auto& GetLocalRight() const { return _local.GetRight(); }
    auto& GetLocalForward() const { return _local.GetForward(); }
    auto& GetLocalPosition() const { return _local.GetPosition(); }
    auto& GetLocalScale() const { return _local.GetScale(); }
    auto& GetLocalRotation() const { return _local.GetRotation(); }
    auto& GetLocalTranslationMatrix() const { return _local.GetTransformMatrix(); }
    auto& GetLocalScaleMatrix() const { return _local.GetTransformMatrix(); }
    auto& GetLocalRotationMatrix() const { return _local.GetTransformMatrix(); }
    auto& GetLocalTransformMatrix() const { return _local.GetTransformMatrix(); }
    void SetLocalPosition(const glm::vec3& a_Val) { _worldNeedsUpdate |= _local.SetPosition(a_Val); }
    void SetLocalScale(const glm::vec3& a_Val) { _worldNeedsUpdate |= _local.SetScale(a_Val); }
    void SetLocalRotation(const glm::quat& a_Val) { _worldNeedsUpdate |= _local.SetRotation(a_Val); }

    void UpdateWorld(const Transform& a_Parent);
    const auto& GetWorld() const { CHECK_UPDATE return _world; }
    auto& GetWorldUp() const { CHECK_UPDATE return _world.GetUp(); }
    auto& GetWorldRight() const { CHECK_UPDATE return _world.GetRight(); }
    auto& GetWorldForward() const { CHECK_UPDATE return _world.GetForward(); }
    auto& GetWorldPosition() const { CHECK_UPDATE return _world.GetPosition(); }
    auto& GetWorldScale() const { CHECK_UPDATE return _world.GetScale(); }
    auto& GetWorldRotation() const { CHECK_UPDATE return _world.GetRotation(); }
    auto& GetWorldTranslationMatrix() const { CHECK_UPDATE return _world.GetTransformMatrix(); }
    auto& GetWorldScaleMatrix() const { CHECK_UPDATE return _world.GetTransformMatrix(); }
    auto& GetWorldRotationMatrix() const { CHECK_UPDATE return _world.GetTransformMatrix(); }
    auto& GetWorldTransformMatrix() const { CHECK_UPDATE return _world.GetTransformMatrix(); }

private:
    bool _worldNeedsUpdate = false;
    TransformData _local;
    TransformData _world;
};

inline void Transform::UpdateWorld(const Transform& a_Parent)
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

}
