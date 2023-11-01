#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

namespace TabGraph::Renderer::RAII {
struct Texture;
struct Sampler;
}

namespace TabGraph::Renderer::RAII {
struct TextureSampler {
    TextureSampler(
        const std::shared_ptr<RAII::Texture>& a_Texture,
        const std::shared_ptr<RAII::Sampler>& a_Sampler);
    ~TextureSampler();
    operator auto() const { return handle; }
    const uint64_t handle = 0;
    std::shared_ptr<RAII::Texture> texture;
    std::shared_ptr<RAII::Sampler> sampler;
};
}
