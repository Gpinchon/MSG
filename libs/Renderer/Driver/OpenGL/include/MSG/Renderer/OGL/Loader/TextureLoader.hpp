#pragma once

#include <MSG/Tools/ObjectCache.hpp>

#include <memory>

namespace MSG {
class OGLContext;
class OGLTexture;
}

namespace MSG {
class Texture;
}

namespace MSG::Renderer {
using TextureCacheKey = Tools::ObjectCacheKey<Texture*>;
using TextureCache    = Tools::ObjectCache<TextureCacheKey, std::shared_ptr<OGLTexture>>;
class TextureLoader {
public:
    std::shared_ptr<OGLTexture> operator()(OGLContext& a_Context, Texture* a_Texture, const bool& a_Sparse);

private:
    TextureCache textureCache;
};
}
