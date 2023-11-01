#pragma once

#include <Tools/ObjectCache.hpp>

#include <memory>

namespace TabGraph::Renderer::RAII {
struct Context;
struct Sampler;
struct Texture;
struct TextureSampler;
}

namespace TabGraph::Renderer {
using TextureSamplerLoaderKey   = Tools::ObjectCacheKey<RAII::Texture*, RAII::Sampler*>;
using TextureSamplerLoaderCache = Tools::ObjectCache<TextureSamplerLoaderKey, std::shared_ptr<RAII::TextureSampler>>;
struct TextureSamplerLoader : TextureSamplerLoaderCache {
    std::shared_ptr<RAII::TextureSampler> Load(
        RAII::Context& a_Context,
        const std::shared_ptr<RAII::Texture>& a_Texture,
        const std::shared_ptr<RAII::Sampler>& a_Sampler);
};
}
