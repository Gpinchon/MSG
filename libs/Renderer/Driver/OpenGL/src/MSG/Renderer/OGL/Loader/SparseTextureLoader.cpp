#include <MSG/Renderer/OGL/Loader/SparseTextureLoader.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/VirtualTexture.hpp>

#include <MSG/Tools/LazyConstructor.hpp>

std::shared_ptr<MSG::Renderer::VirtualTexture> MSG::Renderer::SparseTextureLoader::operator()(Renderer::Impl& a_Rdr, const std::shared_ptr<Texture>& a_Txt)
{
    auto factory = Tools::LazyConstructor([&a_Rdr, &a_Txt] {
        return std::make_shared<VirtualTexture>(
            a_Rdr.textureLoader(a_Rdr.context, a_Txt.get(), true),
            a_Txt);
    });
    return cache.GetOrCreate(a_Txt.get(), factory);
}