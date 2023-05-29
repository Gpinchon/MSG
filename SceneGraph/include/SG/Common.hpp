/*
 * @Author: gpinchon
 * @Date:   2020-08-27 18:48:19
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-01-11 08:47:38
 */

#pragma once

#include <glm/vec3.hpp>

namespace TabGraph::SG::Common {
glm::vec3 Up();
void SetUp(glm::vec3);
glm::vec3 Forward();
void SetForward(glm::vec3);
glm::vec3 Right();
void SetRight(glm::vec3);
/** Default gravity is { 0, -9.81, 0 } */
glm::vec3 Gravity();
void SetGravity(glm::vec3);
}
