#pragma once

namespace TabGraph::Renderer::RAII {
struct Texture {
    Texture();
    virtual ~Texture();
    operator unsigned() const { return handle; }
    const unsigned handle = 0;
};
struct Texture2D : Texture {
    Texture2D(
        unsigned a_Width,
        unsigned a_Height,
        unsigned a_Levels,
        unsigned a_Format);
    const unsigned width  = 0;
    const unsigned height = 0;
    const unsigned levels = 0;
    const unsigned format = 0;
};
}
