#pragma once

#include <glm/fwd.hpp>

namespace TabGraph::SG {
struct Cubemap;
struct Image2D;
}
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
    void UploadLevel(
        const unsigned& a_Level,
        const SG::Image2D& a_Src) const;
    const unsigned width       = 0;
    const unsigned height      = 0;
    const unsigned levels      = 0;
    const unsigned sizedFormat = 0; // GL_RGBA8, GL_RGB8...
};

struct TextureCubemap : Texture {
    TextureCubemap(
        const unsigned& a_Width,
        const unsigned& a_Height,
        const unsigned& a_Levels,
        const unsigned& a_SizedFormat);
    void UploadLevel(
        const unsigned& a_Level,
        const SG::Cubemap& a_Src) const;
    const unsigned width       = 0;
    const unsigned height      = 0;
    const unsigned levels      = 0;
    const unsigned sizedFormat = 0; // GL_RGBA8, GL_RGB8...
};
}
