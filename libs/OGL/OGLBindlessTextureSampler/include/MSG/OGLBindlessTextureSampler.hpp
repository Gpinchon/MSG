#pragma once

#include <GL/glew.h>
#include <glm/vec4.hpp>

#include <memory>

namespace Msg {
class OGLContext;
class OGLTexture;
class OGLSampler;
}

namespace Msg {
/** @brief a class used for bindless texturing */
class OGLBindlessTextureSampler {
public:
    OGLBindlessTextureSampler(
        OGLContext& a_Context,
        const std::shared_ptr<OGLTexture>& a_Texture,
        const std::shared_ptr<OGLSampler>& a_Sampler);
    void MakeResident(const bool& a_Resident = true);
    operator uint64_t() const { return handle; }
    const uint64_t handle = 0;
    const std::shared_ptr<OGLTexture> texture;
    const std::shared_ptr<OGLSampler> sampler;
    OGLContext& context;
};
}
