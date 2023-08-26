#pragma once

namespace TabGraph::Renderer::RAII {
struct Sampler {
    Sampler();
    ~Sampler();
    unsigned handle = 0;
};
}
