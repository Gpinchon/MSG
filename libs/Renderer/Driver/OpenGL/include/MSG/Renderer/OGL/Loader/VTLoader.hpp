#pragma once

#include <MSG/Renderer/OGL/VTPageCache.hpp>
#include <MSG/Renderer/OGL/VTPool.hpp>
#include <MSG/Tools/ObjectCache.hpp>

#include <memory>

namespace Msg::Renderer {
class VirtualTexture;
class Impl;
}

namespace Msg {
class Texture;
class OGLContext;
class OGLTexture;
}

namespace Msg::Renderer {
using VTCacheKey = Tools::ObjectCacheKey<Texture*>;
using VTCache    = Tools::ObjectCache<VTCacheKey, std::shared_ptr<VirtualTexture>>;
class VTLoader {
public:
    VTLoader(OGLContext& a_Ctx);
    void Cleanup();
    std::shared_ptr<OGLTexture> GetAtlas() const;
    std::shared_ptr<VirtualTexture> operator()(Renderer::Impl& a_Rdr, const std::shared_ptr<Texture>& a_Txt);

private:
    VTPool _pool;
    VTCache _cache;
    VTPageCache _pageCache;
};
}