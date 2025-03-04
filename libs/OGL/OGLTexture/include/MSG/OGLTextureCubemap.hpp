#pragma once

#include <MSG/OGLTexture.hpp>

namespace MSG {
class Image;
}

namespace MSG {
class OGLTextureCubemap : public OGLTexture {
public:
    OGLTextureCubemap(
        OGLContext& a_Context,
        const unsigned& a_Width,
        const unsigned& a_Height,
        const unsigned& a_Levels,
        const unsigned& a_SizedFormat);
    void UploadLevel(
        const unsigned& a_Level,
        const Image& a_Src) const;
};
}