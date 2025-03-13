#pragma once

#include <MSG/OGLTexture.hpp>

namespace MSG {
class Image;
}

namespace MSG {
struct OGLTexture3DInfo {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t levels;
    uint32_t sizedFormat;
};
class OGLTexture3D : public OGLTexture {
public:
    OGLTexture3D(
        OGLContext& a_Context,
        const OGLTexture3DInfo& a_Info);
    void UploadLevel(
        const unsigned& a_Level,
        const Image& a_Src) const;
};
}