#pragma once

#include <cstdint>

namespace MSG {
class OGLContext;
}

namespace MSG {
struct OGLTextureInfo {
    uint32_t target;
    uint32_t width  = 1;
    uint32_t height = 1;
    uint32_t depth  = 1;
    uint32_t levels = 1;
    uint32_t sizedFormat; // GL_RGBA8, GL_RGB8...
};
class OGLTexture : public OGLTextureInfo {
public:
    explicit OGLTexture(OGLContext& a_Context,
        const OGLTextureInfo& a_Info);
    virtual ~OGLTexture();
    operator unsigned() const { return handle; }
    const unsigned handle = 0;
    OGLContext& context;
};
}
