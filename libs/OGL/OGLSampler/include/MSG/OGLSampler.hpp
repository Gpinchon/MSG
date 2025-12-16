#pragma once

#include <GL/glew.h>
#include <glm/vec4.hpp>

namespace Msg {
class OGLContext;
}

namespace Msg {
struct OGLSamplerParameters {
    unsigned minFilter    = GL_NEAREST_MIPMAP_LINEAR;
    unsigned magFilter    = GL_LINEAR;
    unsigned wrapS        = GL_REPEAT;
    unsigned wrapT        = GL_REPEAT;
    unsigned wrapR        = GL_REPEAT;
    unsigned compareMode  = GL_NONE;
    unsigned compareFunc  = GL_GEQUAL;
    bool seamlessCubemap  = false;
    float maxAnisotropy   = 1.f;
    float lodBias         = 0.f;
    float minLOD          = -1000;
    float maxLOD          = 1000;
    glm::vec4 borderColor = { 0, 0, 0, 0 };
};
class OGLSampler : public OGLSamplerParameters {
public:
    OGLSampler(OGLContext& a_Context, const OGLSamplerParameters& a_Parameters = {});
    ~OGLSampler();
    void Update(const OGLSamplerParameters& a_Parameters);
    operator unsigned() const { return handle; }
    const unsigned handle = 0;
    OGLContext& context;
};
}
