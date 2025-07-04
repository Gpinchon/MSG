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
    auto parameters = reinterpret_cast<OGLSamplerParameters*>(this);
    ExecuteOGLCommand(context, [handle = handle, thisParameters = *parameters, inParameters = a_Parameters] {
        if (inParameters.minFilter != thisParameters.minFilter)
            glSamplerParameteri(handle, GL_TEXTURE_MIN_FILTER, inParameters.minFilter);
        if (inParameters.magFilter != thisParameters.magFilter)
            glSamplerParameteri(handle, GL_TEXTURE_MAG_FILTER, inParameters.magFilter);
        if (inParameters.wrapS != thisParameters.wrapS)
            glSamplerParameteri(handle, GL_TEXTURE_WRAP_S, inParameters.wrapS);
        if (inParameters.wrapT != thisParameters.wrapT)
            glSamplerParameteri(handle, GL_TEXTURE_WRAP_T, inParameters.wrapT);
        if (inParameters.wrapR != thisParameters.wrapR)
            glSamplerParameteri(handle, GL_TEXTURE_WRAP_R, inParameters.wrapR);
        if (inParameters.compareMode != thisParameters.compareMode)
            glSamplerParameteri(handle, GL_TEXTURE_COMPARE_MODE, inParameters.compareMode);
        if (inParameters.compareFunc != thisParameters.compareFunc)
            glSamplerParameteri(handle, GL_TEXTURE_COMPARE_FUNC, inParameters.compareFunc);
        if (inParameters.maxAnisotropy != thisParameters.maxAnisotropy)
            glSamplerParameterf(handle, GL_TEXTURE_MAX_ANISOTROPY, inParameters.maxAnisotropy);
        if (inParameters.lodBias != thisParameters.lodBias)
            glSamplerParameterf(handle, GL_TEXTURE_LOD_BIAS, inParameters.lodBias);
        if (inParameters.minLOD != thisParameters.minLOD)
            glSamplerParameteri(handle, GL_TEXTURE_MIN_LOD, inParameters.minLOD);
        if (inParameters.maxLOD != thisParameters.maxLOD)
            glSamplerParameteri(handle, GL_TEXTURE_MAX_LOD, inParameters.maxLOD);
        if (inParameters.borderColor != thisParameters.borderColor)
            glSamplerParameterfv(handle, GL_TEXTURE_BORDER_COLOR, &inParameters.borderColor[0]);
    });
    *parameters = a_Parameters;
}
}