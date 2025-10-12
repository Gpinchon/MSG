#pragma once

#include <MSG/OGLTexture.hpp>

#include <cstdint>

namespace Msg {
class OGLContext;
}

namespace Msg {
struct OGLTextureViewInfo {
    uint32_t target; // the view's new target
    uint32_t minLevel  = 0;
    uint32_t numLevels = 1;
    uint32_t minLayer  = 0;
    uint32_t numLayers = 1;
    uint32_t sizedFormat; // a new format to interpret the texture
};
class OGLTextureView : public OGLTexture {
public:
    explicit OGLTextureView(OGLContext& a_Context,
        const std::shared_ptr<OGLTexture>& a_Target,
        const OGLTextureViewInfo& a_Info);
};
}
