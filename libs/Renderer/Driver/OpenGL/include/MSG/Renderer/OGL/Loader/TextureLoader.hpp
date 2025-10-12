#pragma once

#include <MSG/Tools/ObjectCache.hpp>

#include <memory>

namespace Msg {
class OGLContext;
class OGLTexture;
}

namespace Msg {
class Texture;
}

namespace Msg::Renderer {
using TextureCacheKey = Tools::ObjectCacheKey<Texture*>;
using TextureCache    = Tools::ObjectCache<TextureCacheKey, std::shared_ptr<OGLTexture>>;
class TextureLoader {
public:
    std::shared_ptr<OGLTexture> operator()(OGLContext& a_Context, Texture* a_Texture, const bool& a_Sparse);

private:
    TextureCache textureCache;
};
}
