#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <Tools/ObjectCache.hpp>

namespace TabGraph::Renderer {
class Context;
}

namespace TabGraph::Renderer::RAII {
class Sampler;
}

namespace TabGraph::SG {
class Sampler;
}

namespace TabGraph::Renderer {
using SamplerCacheKey = Tools::ObjectCacheKey<SG::Sampler*>;
using SamplerCache    = Tools::ObjectCache<SamplerCacheKey, std::shared_ptr<RAII::Sampler>>;
class SamplerLoader : public SamplerCache {
public:
    std::shared_ptr<RAII::Sampler> operator()(Context& a_Context, SG::Sampler* a_Sampler);
};
}
