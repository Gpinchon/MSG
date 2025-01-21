#pragma once

#include <glm/fwd.hpp>

namespace MSG::Core {
class Cubemap;
class Image2D;
}

namespace MSG::Renderer::RAII {
class Texture {
public:
    explicit Texture(const unsigned& a_Target);
    virtual ~Texture();
    operator unsigned() const { return handle; }
    const unsigned target = 0;
    const unsigned handle = 0;
};

class Texture2D : public Texture {
public:
    Texture2D(
        const unsigned& a_Width,
        const unsigned& a_Height,
        const unsigned& a_Levels,
        const unsigned& a_SizedFormat);
    void UploadLevel(
        const unsigned& a_Level,
        const Core::Image2D& a_Src) const;
    const unsigned width       = 0;
    const unsigned height      = 0;
    const unsigned levels      = 0;
    const unsigned sizedFormat = 0; // GL_RGBA8, GL_RGB8...
};

class TextureCubemap : public Texture {
public:
    TextureCubemap(
        const unsigned& a_Width,
        const unsigned& a_Height,
        const unsigned& a_Levels,
        const unsigned& a_SizedFormat);
    void UploadLevel(
        const unsigned& a_Level,
        const Core::Cubemap& a_Src) const;
    const unsigned width       = 0;
    const unsigned height      = 0;
    const unsigned levels      = 0;
    const unsigned sizedFormat = 0; // GL_RGBA8, GL_RGB8...
};
}
