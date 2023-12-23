#include <Renderer/OGL/Loader/TextureSamplerLoader.hpp>
#include <Renderer/OGL/RAII/TextureSampler.hpp>

#include <Tools/LazyConstructor.hpp>

namespace TabGraph::Renderer {
std::shared_ptr<RAII::TextureSampler> TextureSamplerLoader::Load(
    Context& a_Context,
    const std::shared_ptr<RAII::Texture>& a_Texture,
    const std::shared_ptr<RAII::Sampler>& a_Sampler)
{
    Tools::LazyConstructor factory = [&context = a_Context, a_Texture, a_Sampler] {
        return RAII::MakePtr<RAII::TextureSampler>(context, a_Texture, a_Sampler);
    };
    return GetOrCreate(a_Texture.get(), a_Sampler.get(), factory);
}
}
