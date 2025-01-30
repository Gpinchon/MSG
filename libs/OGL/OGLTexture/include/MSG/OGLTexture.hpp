#pragma once

namespace MSG {
class OGLContext;
}

namespace MSG {
class OGLTexture {
public:
    explicit OGLTexture(OGLContext& a_Context, const unsigned& a_Target);
    virtual ~OGLTexture();
    operator unsigned() const { return handle; }
    const unsigned target = 0;
    const unsigned handle = 0;
    OGLContext& context;
};
}
