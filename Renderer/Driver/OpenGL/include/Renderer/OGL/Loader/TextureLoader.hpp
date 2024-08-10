#pragma once

#include <Tools/ObjectCache.hpp>

#include <memory>

namespace TabGraph::Renderer {
struct Context;
}

namespace TabGraph::Renderer::RAII {
struct Texture;
}

namespace TabGraph::SG {
class Texture;
}

namespace TabGraph::Renderer {
using TextureCacheKey = Tools::ObjectCacheKey<SG::Texture*>;
using TextureCache    = Tools::ObjectCache<TextureCacheKey, std::shared_ptr<RAII::Texture>>;
class TextureLoader {
public:
    std::shared_ptr<RAII::Texture> operator()(Context& a_Context, SG::Texture* a_Texture);

private:
    TextureCache textureCache;
};
}
