/*
 * @Author: gpinchon
 * @Date:   2020-08-27 18:48:19
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-01-11 08:47:38
 */

#pragma once

#include <glm/fwd.hpp>

namespace MSG::Core {
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
