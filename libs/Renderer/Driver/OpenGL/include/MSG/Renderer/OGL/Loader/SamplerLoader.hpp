#pragma once

#include <MSG/Tools/ObjectCache.hpp>

#include <memory>

namespace MSG {
class OGLContext;
class OGLSampler;
}

namespace MSG {
class Sampler;
}

namespace MSG::Renderer {
using SamplerCacheKey = Tools::ObjectCacheKey<Sampler*>;
using SamplerCache    = Tools::ObjectCache<SamplerCacheKey, std::shared_ptr<OGLSampler>>;
class SamplerLoader : public SamplerCache {
public:
    std::shared_ptr<OGLSampler> operator()(OGLContext& a_Context, Sampler* a_Sampler);
};
}
