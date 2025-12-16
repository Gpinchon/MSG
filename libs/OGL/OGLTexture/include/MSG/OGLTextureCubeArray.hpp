#pragma once

#include <MSG/OGLTexture.hpp>

#include <cstdint>

namespace Msg {
class Image;
}

namespace Msg {
struct OGLTextureCubeArrayInfo {
    uint32_t width;
    uint32_t height;
    uint32_t layers;
    uint32_t levels;
    uint32_t sizedFormat;
    bool sparse = false;
};
class OGLTextureCubeArray : public OGLTexture {
public:
    OGLTextureCubeArray(
        OGLContext& a_Context,
        const OGLTextureCubeArrayInfo& a_Info);
};
}