#pragma once

#include <MSG/PixelDescriptor.hpp>

#include <cstdint>

namespace Msg {
class OGLContext;
class Image;
}

namespace Msg {
struct OGLTextureInfo {
    uint32_t target;
    uint32_t width  = 1;
    uint32_t height = 1;
    uint32_t depth  = 1;
    uint32_t levels = 1;
    uint32_t sizedFormat; // GL_RGBA8, GL_RGB8...
    bool sparse = false;
};
struct OGLTextureUploadInfo {
    uint32_t level   = 0;
    uint32_t offsetX = 0;
    uint32_t offsetY = 0;
    uint32_t offsetZ = 0;
    uint32_t width   = 1;
    uint32_t height  = 1;
    uint32_t depth   = 1;
    PixelDescriptor pixelDescriptor;
};
struct OGLTextureCommitInfo {
    uint32_t level   = 0;
    uint32_t offsetX = 0;
    uint32_t offsetY = 0;
    uint32_t offsetZ = 0;
    uint32_t width   = 1;
    uint32_t height  = 1;
    uint32_t depth   = 1;
    bool commit      = true;
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
    void CommitPage(const OGLTextureCommitInfo& a_Info);
    void Allocate();
    void Clear(
        const uint32_t& a_Format,
        const uint32_t& a_Type,
        const void* a_Data) const;
    void UploadLevel(
        const unsigned& a_Level,
        const Image& a_Src) const;
    void UploadLevel(
        const unsigned& a_Level,
        const glm::uvec3& a_SrcOffset,
        const glm::uvec3& a_SrcSize,
        const Image& a_Src) const;
    void UploadLevel(
        const OGLTextureUploadInfo& a_Info,
        std::vector<std::byte> a_Data) const;
    operator unsigned() const { return handle; }
    unsigned handle = 0;
    OGLContext& context;
};
}
