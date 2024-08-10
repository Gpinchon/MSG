#include <Renderer/OGL/Loader/SamplerLoader.hpp>
#include <Renderer/OGL/RAII/Sampler.hpp>
#include <Renderer/OGL/ToGL.hpp>

#include <SG/Core/Texture/Sampler.hpp>
#include <Tools/LazyConstructor.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer {
std::shared_ptr<RAII::Sampler> SamplerLoader::operator()(Context& a_Context, SG::Sampler* a_Sampler)
{
    auto factory = Tools::LazyConstructor([&context = a_Context, sampler = a_Sampler] {
        return RAII::MakePtr<RAII::Sampler>(context, ToGL(*sampler));
    });
    return GetOrCreate(a_Sampler, factory);
}
}
