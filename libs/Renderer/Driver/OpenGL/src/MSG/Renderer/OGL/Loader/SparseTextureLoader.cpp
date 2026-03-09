#include <MSG/Renderer/OGL/Loader/SparseTextureLoader.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/SparseTexture.hpp>

#include <MSG/Tools/LazyConstructor.hpp>

void Msg::Renderer::SparseTextureLoader::Cleanup()
{
    auto itr = _cache.begin();
    while (itr != _cache.end()) {
        if (itr->second.use_count() == 1)
            itr = _cache.erase(itr);
        else
            itr++;
    }
    _pageCache.Cleanup();
}

std::shared_ptr<Msg::Renderer::SparseTexture> Msg::Renderer::SparseTextureLoader::operator()(Renderer::Impl& a_Rdr, const std::shared_ptr<Texture>& a_Txt)
{
    auto factory = Tools::LazyConstructor([this, &a_Rdr, &a_Txt] {
        return std::make_shared<SparseTexture>(a_Rdr.context, a_Txt, _pageCache);
    });
    return _cache.GetOrCreate(a_Txt.get(), factory);
}