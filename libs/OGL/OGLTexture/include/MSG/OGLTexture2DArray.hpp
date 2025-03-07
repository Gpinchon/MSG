#pragma once

#include <MSG/OGLTexture.hpp>

namespace MSG {
class Image;
}

namespace MSG {
struct OGLTexture2DArrayInfo {
    uint32_t width;
    uint32_t height;
    uint32_t layers;
    uint32_t levels;
    uint32_t sizedFormat;
};
class OGLTexture2DArray : public OGLTexture {
public:
    OGLTexture2DArray(
        OGLContext& a_Context,
        const OGLTexture2DArrayInfo& a_Info);
    void UploadLevel(
        const unsigned& a_Level,
        const Image& a_Src) const;
};
}