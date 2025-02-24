#pragma once

#include <glm/mat4x4.hpp>

#include <memory>

namespace MSG {
class OGLTexture;
}

namespace MSG::Renderer::Component {
class ShadowCaster {
    glm::mat4x4 projection;
    std::shared_ptr<OGLTexture> shadowMap;
};
}