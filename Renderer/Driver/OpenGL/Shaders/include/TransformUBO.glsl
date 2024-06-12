#pragma once

#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>

struct TransformUBO {
    glm::mat4 matrix;
};