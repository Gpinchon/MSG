#pragma once

#include <MSG/OGLTexture.hpp>

namespace MSG {
class Image;
}

namespace MSG {
struct OGLTexture2DInfo {
    uint32_t width  = 1;
    uint32_t height = 1;
    uint32_t levels = 1;
    uint32_t sizedFormat;
};
class OGLTexture2D : public OGLTexture {
public:
    OGLTexture2D(
        OGLContext& a_Context,
        const OGLTexture2DInfo& a_Info);
    void UploadLevel(
        const unsigned& a_Level,
        const Image& a_Src) const;
};
}