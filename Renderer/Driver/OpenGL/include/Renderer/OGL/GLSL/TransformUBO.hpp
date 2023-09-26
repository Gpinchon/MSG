#pragma once

#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>

struct TransformUBO {
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;
    glm::mat4 transform;
};