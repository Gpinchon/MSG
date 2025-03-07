#pragma once

#include <MSG/OGLTexture.hpp>

#include <cstdint>

namespace MSG {
class Image;
}

namespace MSG {
struct OGLTextureCubeInfo {
    uint32_t width;
    uint32_t height;
    uint32_t levels;
    uint32_t sizedFormat;
};
class OGLTextureCube : public OGLTexture {
public:
    OGLTextureCube(
        OGLContext& a_Context,
        const OGLTextureCubeInfo& a_Info);
    void UploadLevel(
        const unsigned& a_Level,
        const Image& a_Src) const;
};
}