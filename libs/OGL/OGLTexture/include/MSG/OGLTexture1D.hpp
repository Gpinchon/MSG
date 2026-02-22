#pragma once

#include <MSG/OGLTexture.hpp>
#include <MSG/PixelDescriptor.hpp>

namespace Msg {
class Image;
}

namespace Msg {
struct OGLTexture1DInfo {
    uint32_t width  = 1;
    uint32_t levels = 1;
    uint32_t sizedFormat;
};

class OGLTexture1D : public OGLTexture {
public:
    OGLTexture1D(
        OGLContext& a_Context,
        const OGLTexture1DInfo& a_Info);
};
}