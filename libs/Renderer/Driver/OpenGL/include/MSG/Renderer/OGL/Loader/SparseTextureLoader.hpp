#pragma once

#include <MSG/Renderer/OGL/SparseTexturePageCache.hpp>
#include <MSG/Tools/ObjectCache.hpp>

#include <memory>

namespace Msg::Renderer {
class SparseTexture;
class Impl;
}

namespace Msg {
class Texture;
}

namespace Msg::Renderer {
using SparseTextureCacheKey = Tools::ObjectCacheKey<Texture*>;
using SparseTextureCache    = Tools::ObjectCache<SparseTextureCacheKey, std::shared_ptr<SparseTexture>>;
class SparseTextureLoader {
public:
    std::shared_ptr<SparseTexture> operator()(Renderer::Impl& a_Rdr, const std::shared_ptr<Texture>& a_Txt);

private:
    SparseTextureCache _cache;
    SparseTexturePageCache _pageCache;
};
}