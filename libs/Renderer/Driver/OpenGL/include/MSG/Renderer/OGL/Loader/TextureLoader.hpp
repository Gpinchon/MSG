#pragma once

#include <MSG/Tools/ObjectCache.hpp>

#include <memory>

namespace MSG {
class OGLContext;
}

namespace MSG::Renderer::RAII {
class Texture;
}

namespace MSG {
class Texture;
}

namespace MSG::Renderer {
using TextureCacheKey = Tools::ObjectCacheKey<Texture*>;
using TextureCache    = Tools::ObjectCache<TextureCacheKey, std::shared_ptr<RAII::Texture>>;
class TextureLoader {
public:
    std::shared_ptr<RAII::Texture> operator()(OGLContext& a_Context, Texture* a_Texture);

private:
    TextureCache textureCache;
};
}
