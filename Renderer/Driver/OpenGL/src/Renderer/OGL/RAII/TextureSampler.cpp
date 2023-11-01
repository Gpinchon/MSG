#include <Renderer/OGL/RAII/TextureSampler.hpp>

#include <Renderer/OGL/RAII/Sampler.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer::RAII {
TextureSampler::TextureSampler(const std::shared_ptr<RAII::Texture>& a_Texture, const std::shared_ptr<RAII::Sampler>& a_Sampler)
    : handle(glGetTextureSamplerHandleARB(*a_Texture, *a_Sampler))
    , texture(a_Texture)
    , sampler(a_Sampler)
{
    glMakeTextureHandleResidentARB(handle);
}
TextureSampler::~TextureSampler()
{
    glMakeTextureHandleNonResidentARB(handle);
}
}
