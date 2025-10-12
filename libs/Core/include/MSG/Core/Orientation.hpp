#pragma once

#include <glm/fwd.hpp>

namespace Msg::Core {
const glm::vec3& Up();
void SetUp(const glm::vec3&);
const glm::vec3& Forward();
void SetForward(const glm::vec3&);
const glm::vec3& Right();
void SetRight(const glm::vec3&);
/** Default gravity is { 0, -9.81, 0 } */
const glm::vec3& Gravity();
void SetGravity(const glm::vec3&);
}
