#include <MSG/OGLContext.hpp>
#include <MSG/OGLSampler.hpp>

#include <GL/glew.h>

namespace MSG {
static inline auto CreateSampler(OGLContext& a_Context)
{
    unsigned handle = 0;
    ExecuteOGLCommand(a_Context, [&handle] { glCreateSamplers(1, &handle); }, true);
    return handle;
}

OGLSampler::OGLSampler(OGLContext& a_Context, const OGLSamplerParameters& a_Parameters)
    : handle(CreateSampler(a_Context))
    , context(a_Context)
{
    Update(a_Parameters);
}

OGLSampler::~OGLSampler()
{
    ExecuteOGLCommand(context, [handle = handle] { glDeleteSamplers(1, &handle); });
}

void OGLSampler::Update(const OGLSamplerParameters& a_Parameters)
{
    ExecuteOGLCommand(context, [handle = handle, parameters = parameters, a_Parameters] {
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
        if (a_Parameters.borderColor != parameters.borderColor)
            glSamplerParameterfv(handle, GL_TEXTURE_BORDER_COLOR, &a_Parameters.borderColor[0]);
    });
    parameters = a_Parameters;
}
}