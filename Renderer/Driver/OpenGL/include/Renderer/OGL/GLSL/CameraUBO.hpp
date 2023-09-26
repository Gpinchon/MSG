#pragma once

//#include <Renderer/OGL/GLSL/TransformUBO.hpp>

struct CameraUBO {
    //TransformUBO transform;
    glm::mat4 projection;
    glm::mat4 view;
};