#pragma once

#include <MSG/Tools/ObjectCache.hpp>

#include <memory>

namespace MSG::Renderer {
class VirtualTexture;
class Impl;
}

namespace MSG {
class Texture;
}

namespace MSG::Renderer {
using SparseTextureCacheKey = Tools::ObjectCacheKey<Texture*>;
using SparseTextureCache    = Tools::ObjectCache<SparseTextureCacheKey, std::shared_ptr<VirtualTexture>>;
class SparseTextureLoader {
public:
    std::shared_ptr<VirtualTexture> operator()(Renderer::Impl& a_Rdr, const std::shared_ptr<Texture>& a_Txt);

private:
    SparseTextureCache cache;
};
}