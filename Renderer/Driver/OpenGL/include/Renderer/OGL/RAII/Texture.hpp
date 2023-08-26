#pragma once

namespace TabGraph::Renderer::RAII {
struct Texture2D {
    Texture2D(
        unsigned a_Width,
        unsigned a_Height,
        unsigned a_Levels,
        unsigned a_Format);
    ~Texture2D();
    unsigned handle       = 0;
    const unsigned width  = 0;
    const unsigned height = 0;
    const unsigned levels = 0;
    const unsigned format = 0;
};
}
