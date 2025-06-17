#pragma once

#include <MSG/OGLTexture.hpp>

namespace MSG {
class Image;
}

namespace MSG {
struct OGLTexture3DInfo {
    uint32_t width  = 1;
    uint32_t height = 1;
    uint32_t depth  = 1;
    uint32_t levels = 1;
    uint32_t sizedFormat;
};
class OGLTexture3D : public OGLTexture {
public:
    OGLTexture3D(
        OGLContext& a_Context,
        const OGLTexture3DInfo& a_Info);
};
}