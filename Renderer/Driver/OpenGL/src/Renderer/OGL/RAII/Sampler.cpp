#include <Renderer/OGL/RAII/Sampler.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer::RAII {
Sampler::Sampler()
{
    glCreateSamplers(1, &handle);
}

Sampler::~Sampler()
{
    glDeleteSamplers(1, &handle);
}
}
