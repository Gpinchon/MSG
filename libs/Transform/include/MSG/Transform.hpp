#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Core/Orientation.hpp>
#include <MSG/Tools/Debug.hpp>
#include <MSG/Transform/Data.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
#ifndef NDEBUG
#define CHECK_UPDATE                                \
    {                                               \
        if (_worldNeedsUpdate) {                    \
            debugLog("World transform outdated !"); \
            throw std::runtime_error("");           \
        }                                           \
    }
#else
#define CHECK_UPDATE
#endif

class Transform {
public:
    void LookAt(const glm::vec3& a_Target);
    void LookAt(const Transform& a_Target);

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

    void UpdateWorld(const Transform& a_Parent = {});
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
}
