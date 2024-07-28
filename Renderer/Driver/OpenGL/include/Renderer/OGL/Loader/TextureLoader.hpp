#pragma once

#include <Functions.glsl>

#include <Tools/ObjectCache.hpp>

#include <memory>

namespace TabGraph::Renderer {
struct Context;
}

namespace TabGraph::Renderer::RAII {
struct Texture;
struct Texture1D;
struct Texture2D;
struct Texture3D;
struct TextureCubemap;
}

namespace TabGraph::SG {
struct Image;
struct Cubemap;
}

namespace TabGraph::Renderer {
using Texture1DCacheKey      = Tools::ObjectCacheKey<SG::Image*>;
using Texture2DCacheKey      = Tools::ObjectCacheKey<SG::Image*>;
using Texture3DCacheKey      = Tools::ObjectCacheKey<SG::Image*>;
using TextureCubemapCacheKey = Tools::ObjectCacheKey<SG::Image*>;
using Texture1DCache         = Tools::ObjectCache<Texture1DCacheKey, std::shared_ptr<RAII::Texture1D>>;
using Texture2DCache         = Tools::ObjectCache<Texture2DCacheKey, std::shared_ptr<RAII::Texture2D>>;
using Texture3DCache         = Tools::ObjectCache<Texture3DCacheKey, std::shared_ptr<RAII::Texture3D>>;
using TextureCubemapCache    = Tools::ObjectCache<TextureCubemapCacheKey, std::shared_ptr<RAII::TextureCubemap>>;
class TextureLoader {
public:
    std::shared_ptr<RAII::Texture> operator()(Context& a_Context, SG::Image* a_Image, const uint& a_Mips = 1);

private:
    Texture1DCache texture1DCache;
    Texture2DCache texture2DCache;
    Texture3DCache texture3DCache;
    TextureCubemapCache textureCubemapCache;
};
}
