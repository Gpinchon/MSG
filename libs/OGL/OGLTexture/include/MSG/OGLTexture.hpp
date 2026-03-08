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
// TODO remove PixelDescriptor
//  Replace with dataFormat, dataType and compressed
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
struct OGLTextureFormatSparseInfo {
    bool supported       = false;
    uint32_t sizedFormat = 0;
    int32_t pageWidth    = 0;
    int32_t pageHeight   = 0;
    int32_t pageDepth    = 0;
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
    explicit OGLTexture(OGLContext& a_Context);
    explicit OGLTexture(OGLTexture&& a_Other);
    virtual ~OGLTexture();
    void Initialize(const OGLTextureInfo& a_Info);
    void GenerateMipmap() const;
    void CommitPage(const OGLTextureCommitInfo& a_Info);
    void Allocate();
    void Clear(
        const uint32_t& a_Format,
        const uint32_t& a_Type,
        const uint32_t& a_Level,
        const void* a_Data) const;
    void DownloadLevel(
        const uint32_t& a_Level,
        const uint32_t& a_Format, // GL_RGB, GL_RED, GL_DEPTH_COMPONENT...
        const uint32_t& a_Type, // GL_SHORT, GL_FLOAT...
        const uint32_t& a_BufferSize,
        void* a_Data) const;
    void UploadLevel(
        const uint32_t& a_Level,
        const Image& a_Src) const;
    void UploadLevel(
        const uint32_t& a_Level,
        const glm::uvec3& a_SrcOffset,
        const glm::uvec3& a_SrcSize,
        const Image& a_Src) const;
    void UploadLevel(
        const OGLTextureUploadInfo& a_Info,
        std::vector<std::byte> a_Data) const;
    operator uint32_t() const { return handle; }
    static uint32_t Create(OGLContext& a_Context, const uint32_t& a_Target);
    static OGLTextureFormatSparseInfo GetFormatSparseInfo(OGLContext& a_Context, const uint32_t& a_TextureTarget, const uint32_t& a_SizedFormat);
    uint32_t handle       = 0;
    uint32_t sparseLevels = 0; // the number of sparse levels if texture is sparse, 0 otherwise
    OGLContext& context;
};
}
