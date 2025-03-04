#pragma once

namespace MSG {
class OGLContext;
}

namespace MSG {
class OGLTexture {
public:
    explicit OGLTexture(OGLContext& a_Context,
        const unsigned& a_Target, const unsigned& a_SizedFormat,
        const unsigned& a_Width, const unsigned& a_Height, const unsigned& a_Depth,
        const unsigned& a_Levels);
    virtual ~OGLTexture();
    operator unsigned() const { return handle; }
    const unsigned handle      = 0;
    const unsigned target      = 0;
    const unsigned sizedFormat = 0; // GL_RGBA8, GL_RGB8...
    const unsigned width       = 0;
    const unsigned height      = 0;
    const unsigned depth       = 0;
    const unsigned levels      = 0;
    OGLContext& context;
};
}
