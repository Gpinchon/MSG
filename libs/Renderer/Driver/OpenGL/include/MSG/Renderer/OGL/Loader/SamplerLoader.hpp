#pragma once

#include <MSG/Renderer/OGL/RAII/Wrapper.hpp>

#include <MSG/Tools/ObjectCache.hpp>

namespace MSG {
class OGLContext;
}

namespace MSG::Renderer::RAII {
class Sampler;
}

namespace MSG {
class Sampler;
}

namespace MSG::Renderer {
using SamplerCacheKey = Tools::ObjectCacheKey<Sampler*>;
using SamplerCache    = Tools::ObjectCache<SamplerCacheKey, std::shared_ptr<RAII::Sampler>>;
class SamplerLoader : public SamplerCache {
public:
    std::shared_ptr<RAII::Sampler> operator()(OGLContext& a_Context, Sampler* a_Sampler);
};
}
