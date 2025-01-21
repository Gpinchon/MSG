#pragma once

#include <Tools/ObjectCache.hpp>

#include <memory>

namespace MSG::Renderer {
class Context;
}

namespace MSG::Renderer::RAII {
class Texture;
}

namespace MSG::Core {
class Texture;
}

namespace MSG::Renderer {
using TextureCacheKey = Tools::ObjectCacheKey<Core::Texture*>;
using TextureCache    = Tools::ObjectCache<TextureCacheKey, std::shared_ptr<RAII::Texture>>;
class TextureLoader {
public:
    std::shared_ptr<RAII::Texture> operator()(Context& a_Context, Core::Texture* a_Texture);

private:
    TextureCache textureCache;
};
}
