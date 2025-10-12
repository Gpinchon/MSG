#pragma once

#include <MSG/OGLTexture.hpp>

namespace Msg {
class Image;
}

namespace Msg {
struct OGLTexture2DArrayInfo {
    uint32_t width  = 1;
    uint32_t height = 1;
    uint32_t layers = 1;
    uint32_t levels = 1;
    uint32_t sizedFormat;
};
class OGLTexture2DArray : public OGLTexture {
public:
    OGLTexture2DArray(
        OGLContext& a_Context,
        const OGLTexture2DArrayInfo& a_Info);
};
}