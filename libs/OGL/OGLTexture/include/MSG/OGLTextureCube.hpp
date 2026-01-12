#pragma once

#include <MSG/OGLTexture.hpp>

#include <cstdint>

namespace Msg {
class Image;
}

namespace Msg {
struct OGLTextureCubeInfo {
    uint32_t width       = 0;
    uint32_t height      = 0;
    uint32_t levels      = 1;
    uint32_t sizedFormat = 0;
    bool sparse          = false;
};
class OGLTextureCube : public OGLTexture {
public:
    OGLTextureCube(
        OGLContext& a_Context,
        const OGLTextureCubeInfo& a_Info);
};
}