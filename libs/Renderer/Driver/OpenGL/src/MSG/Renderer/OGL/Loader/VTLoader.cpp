#include <MSG/Renderer/OGL/Loader/VTLoader.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/VirtualTexture.hpp>

#include <MSG/Tools/LazyConstructor.hpp>

Msg::Renderer::VTLoader::VTLoader(OGLContext& a_Ctx)
    : _pool(a_Ctx, GL_RGBA8)
{
}

void Msg::Renderer::VTLoader::Cleanup()
{
    auto itr = _cache.begin();
    while (itr != _cache.end()) {
        if (itr->second.use_count() == 1)
            itr = _cache.erase(itr);
        else
            itr++;
    }
}

std::shared_ptr<Msg::OGLTexture> Msg::Renderer::VTLoader::GetAtlas() const
{
    return _pool.GetAtlas();
}

std::shared_ptr<Msg::Renderer::VirtualTexture> Msg::Renderer::VTLoader::operator()(Renderer::Impl& a_Rdr, const std::shared_ptr<Texture>& a_Txt)
{
    auto factory = Tools::LazyConstructor([this, &a_Rdr, &a_Txt] {
        return std::make_shared<VirtualTexture>(a_Rdr.context, a_Txt, _pageCache, _pool);
    });
    return _cache.GetOrCreate(a_Txt.get(), factory);
}