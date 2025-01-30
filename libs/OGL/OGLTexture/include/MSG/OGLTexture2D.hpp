#pragma once

#include <MSG/OGLTexture.hpp>

namespace MSG {
class Image2D;
}

namespace MSG {
class OGLTexture2D : public OGLTexture {
public:
    OGLTexture2D(
        OGLContext& a_Context,
        const unsigned& a_Width,
        const unsigned& a_Height,
        const unsigned& a_Levels,
        const unsigned& a_SizedFormat);
    void UploadLevel(
        const unsigned& a_Level,
        const Image2D& a_Src) const;
    const unsigned width       = 0;
    const unsigned height      = 0;
    const unsigned levels      = 0;
    const unsigned sizedFormat = 0; // GL_RGBA8, GL_RGB8...
};
}