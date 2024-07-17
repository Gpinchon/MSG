#pragma once

namespace TabGraph::Renderer::RAII {
struct Texture {
    explicit Texture(const unsigned& a_Target);
    virtual ~Texture();
    operator unsigned() const { return handle; }
    const unsigned target = 0;
    const unsigned handle = 0;
};
struct Texture2D : Texture {
    Texture2D(
        const unsigned& a_Width,
        const unsigned& a_Height,
        const unsigned& a_Levels,
        const unsigned& a_SizedFormat);
    const unsigned width       = 0;
    const unsigned height      = 0;
    const unsigned levels      = 0;
    const unsigned sizedFormat = 0; // GL_RGBA8, GL_RGB8...
};
}
