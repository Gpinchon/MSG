#pragma once

#include <glm/vec4.hpp>

namespace TabGraph::Renderer::RAII {
struct SamplerParameters {
    SamplerParameters();
    unsigned minFilter, magFilter;
    unsigned wrapS, wrapT, wrapR;
    unsigned compareMode, compareFunc;
    int minLOD, maxLOD;
    glm::vec4 borderColor;
};
class Sampler {
public:
    Sampler(const SamplerParameters& a_Parameters = {});
    ~Sampler();
    void Update(const SamplerParameters& a_Parameters);
    operator unsigned() const { return handle; }
    const unsigned handle = 0;
    SamplerParameters parameters;
};
}
