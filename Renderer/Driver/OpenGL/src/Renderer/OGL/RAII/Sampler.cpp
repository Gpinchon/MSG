#include <Renderer/OGL/RAII/Sampler.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer::RAII {
static inline auto CreateSampler()
{
    unsigned handle = 0;
    glCreateSamplers(1, &handle);
    return handle;
}

Sampler::Sampler(const SamplerParameters& a_Parameters)
    : handle(CreateSampler())
{
    Update(a_Parameters);
}

Sampler::~Sampler()
{
    glDeleteSamplers(1, &handle);
}

void Sampler::Update(const SamplerParameters& a_Parameters)
{
    if (a_Parameters.minFilter != parameters.minFilter)
        glSamplerParameteri(handle, GL_TEXTURE_MIN_FILTER, a_Parameters.minFilter);
    if (a_Parameters.magFilter != parameters.magFilter)
        glSamplerParameteri(handle, GL_TEXTURE_MAG_FILTER, a_Parameters.magFilter);
    if (a_Parameters.wrapS != parameters.wrapS)
        glSamplerParameteri(handle, GL_TEXTURE_WRAP_S, a_Parameters.wrapS);
    if (a_Parameters.wrapT != parameters.wrapT)
        glSamplerParameteri(handle, GL_TEXTURE_WRAP_T, a_Parameters.wrapT);
    if (a_Parameters.wrapR != parameters.wrapR)
        glSamplerParameteri(handle, GL_TEXTURE_WRAP_R, a_Parameters.wrapR);
    if (a_Parameters.compareMode != parameters.compareMode)
        glSamplerParameteri(handle, GL_TEXTURE_COMPARE_MODE, a_Parameters.compareMode);
    if (a_Parameters.compareFunc != parameters.compareFunc)
        glSamplerParameteri(handle, GL_TEXTURE_COMPARE_FUNC, a_Parameters.compareFunc);
    if (a_Parameters.minLOD != parameters.minLOD)
        glSamplerParameteri(handle, GL_TEXTURE_MIN_LOD, a_Parameters.minLOD);
    if (a_Parameters.maxLOD != parameters.maxLOD)
        glSamplerParameteri(handle, GL_TEXTURE_MAX_LOD, a_Parameters.maxLOD);
    if (a_Parameters.borderColor != a_Parameters.borderColor)
        glSamplerParameterfv(handle, GL_TEXTURE_BORDER_COLOR, &a_Parameters.borderColor[0]);
    parameters = a_Parameters;
}

SamplerParameters::SamplerParameters()
    : minFilter(GL_NEAREST_MIPMAP_LINEAR)
    , magFilter(GL_LINEAR)
    , minLOD(-1000)
    , maxLOD(1000)
    , wrapS(GL_REPEAT)
    , wrapT(GL_REPEAT)
    , wrapR(GL_REPEAT)
    , borderColor(0, 0, 0, 0)
    , compareMode(GL_NONE)
    , compareFunc(GL_GEQUAL)
{
}
}
