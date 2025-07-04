#pragma once

#include <MSG/Tools/ObjectCache.hpp>

#include <memory>

namespace MSG::Renderer {
class SparseTexture;
class Impl;
}

namespace MSG {
class Texture;
}

namespace MSG::Renderer {
using SparseTextureCacheKey = Tools::ObjectCacheKey<Texture*>;
using SparseTextureCache    = Tools::ObjectCache<SparseTextureCacheKey, std::shared_ptr<SparseTexture>>;
class SparseTextureLoader {
public:
    std::shared_ptr<SparseTexture> operator()(Renderer::Impl& a_Rdr, const std::shared_ptr<Texture>& a_Txt);

private:
    SparseTextureCache cache;
};
}