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
    /**
     * @brief Construct a new OGLTexture object
     *
     * @param a_Context the context to use to create this texture
     * @param a_Info the informations required for the new texture's creation
     * @param a_Allocate should we request a new handle and allocate on construction ?
     */
    explicit OGLTexture(OGLContext& a_Context, const OGLTextureInfo& a_Info, const bool& a_Allocate = true);
    virtual ~OGLTexture();
    void Allocate();
    void Clear(
        const uint32_t& a_Format,
        const uint32_t& a_Type,
        const void* a_Data) const;
    operator unsigned() const { return handle; }
    unsigned handle = 0;
    OGLContext& context;
};
}
