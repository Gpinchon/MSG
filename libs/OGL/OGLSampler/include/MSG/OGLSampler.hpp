#pragma once

#include <glm/vec4.hpp>

namespace MSG {
class OGLContext;
}

namespace MSG {
struct OGLSamplerParameters {
    OGLSamplerParameters();
    unsigned minFilter, magFilter;
    unsigned wrapS, wrapT, wrapR;
    unsigned compareMode, compareFunc;
    float minLOD, maxLOD;
    glm::vec4 borderColor;
};
class OGLSampler {
public:
    OGLSampler(OGLContext& a_Context, const OGLSamplerParameters& a_Parameters = {});
    ~OGLSampler();
    void Update(const OGLSamplerParameters& a_Parameters);
    operator unsigned() const { return handle; }
    const unsigned handle = 0;
    OGLSamplerParameters parameters;
    OGLContext& context;
};
}
