#include <MSG/Debug.hpp>
#include <MSG/Renderer/OGL/Loader/VTLoader.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/VirtualTexture.hpp>
#include <MSG/Tools/LazyConstructor.hpp>

Msg::Renderer::VTLoader::~VTLoader()
{
    for (auto& vt : _cache)
        MSGCheckErrorFatal(vt.second.use_count() > 1, "Virtual texture outlived cache !");
}

void Msg::Renderer::VTLoader::Update()
{
    // remove textures we're the only ones holding reference to
    auto itr = _cache.begin();
    while (itr != _cache.end()) {
        if (itr->second.use_count() == 1)
            itr = _cache.erase(itr);
        else
            itr++;
    }
}

void Msg::Renderer::VTLoader::SetPageCount(OGLContext& a_Ctx, const uint32_t& a_PageCount)
{
    // restore allocated pages to the pool
    for (auto& vt : _cache)
        vt.second->Clear();
    _pool.Allocate(a_Ctx, a_PageCount, GL_RGBA8);
    // allocate to the new atlas
    for (auto& vt : _cache)
        vt.second->Allocate();
}

std::shared_ptr<Msg::OGLTexture> Msg::Renderer::VTLoader::GetAtlas() const
{
    return _pool.GetAtlas();
}

std::shared_ptr<Msg::Renderer::VirtualTexture> Msg::Renderer::VTLoader::operator()(
    Renderer::Impl& a_Rdr,
    const std::shared_ptr<Texture>& a_Txt,
    const SamplerWrap& a_WrapS, const SamplerWrap& a_WrapT)
{
    auto factory = Tools::LazyConstructor([this, &a_Rdr, &a_Txt, &a_WrapS, &a_WrapT] {
        return std::make_shared<VirtualTexture>(a_Rdr.context, a_Txt, a_WrapS, a_WrapT, _pageCache, _pool);
    });
    return _cache.GetOrCreate(a_Txt.get(), a_WrapS, a_WrapT, factory);
}