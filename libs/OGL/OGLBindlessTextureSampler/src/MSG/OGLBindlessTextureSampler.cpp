#include <MSG/OGLBindlessTextureSampler.hpp>

#include <MSG/OGLContext.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture.hpp>

#include <GL/glew.h>

namespace Msg {
static inline auto GetHandle(OGLContext& a_Context,
    const std::shared_ptr<OGLTexture>& a_Texture,
    const std::shared_ptr<OGLSampler>& a_Sampler)
{
    uint64_t handle = 0;
    if (a_Sampler == nullptr)
        ExecuteOGLCommand(a_Context, [&texture = *a_Texture, &sampler = *a_Sampler, &handle] { handle = glGetTextureHandleARB(texture); }, true);
    else
        ExecuteOGLCommand(a_Context, [&texture = *a_Texture, &sampler = *a_Sampler, &handle] { handle = glGetTextureSamplerHandleARB(texture, sampler); }, true);
    return handle;
}

OGLBindlessTextureSampler::OGLBindlessTextureSampler(
    OGLContext& a_Context,
    const std::shared_ptr<OGLTexture>& a_Texture,
    const std::shared_ptr<OGLSampler>& a_Sampler)
    : handle(GetHandle(a_Context, a_Texture, a_Sampler))
    , texture(a_Texture)
    , sampler(a_Sampler)
    , context(a_Context)
{
}

void Msg::OGLBindlessTextureSampler::MakeResident(const bool& a_Resident)
{
    if (a_Resident)
        ExecuteOGLCommand(context, [handle = handle] {
            if (!glIsTextureHandleResidentARB(handle))
                glMakeTextureHandleResidentARB(handle);
        });
    else
        ExecuteOGLCommand(context, [handle = handle] {
            if (glIsTextureHandleResidentARB(handle))
                glMakeTextureHandleNonResidentARB(handle);
        });
}
}