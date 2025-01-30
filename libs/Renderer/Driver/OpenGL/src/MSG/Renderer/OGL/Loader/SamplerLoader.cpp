#include <MSG/OGLSampler.hpp>
#include <MSG/Renderer/OGL/Loader/SamplerLoader.hpp>
#include <MSG/Renderer/OGL/ToGL.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/Tools/LazyConstructor.hpp>

#include <GL/glew.h>

namespace MSG::Renderer {
std::shared_ptr<OGLSampler> SamplerLoader::operator()(OGLContext& a_Context, Sampler* a_Sampler)
{
    auto factory = Tools::LazyConstructor([&context = a_Context, sampler = a_Sampler] {
        return std::make_shared<OGLSampler>(context, ToGL(*sampler));
    });
    return GetOrCreate(a_Sampler, factory);
}
}
