#pragma once

#include <MSG/Component.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLFence.hpp>
#include <MSG/OGLTypedBuffer.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <memory>
#include <optional>
#include <variant>

namespace Msg {
enum class LightType;
struct LightShadowSettings;
class OGLBindlessTextureSampler;
class OGLFrameBuffer;
class OGLTexture;
template <typename>
class OGLTypedBufferArray;
}

namespace Msg::Renderer {
class Impl;
}

namespace Msg::Renderer {
struct LightShadowData : Msg::Component {
    LightShadowData(Renderer::Impl& a_Rdr);
    void Update(Renderer::Impl& a_Rdr,
        const LightType& a_LightType,
        const LightShadowSettings& a_ShadowSettings,
        const size_t& a_ViewportCount);
    void UpdateDepthRange(Renderer::Impl& a_Rdr,
        const LightType& a_LightType);
    float minDepth   = 0;
    float maxDepth   = 1;
    bool needsUpdate = false;
    std::shared_ptr<OGLBindlessTextureSampler> textureSampler;
    std::shared_ptr<OGLTexture> textureHZB;
    // used for shadow rendering
    std::shared_ptr<OGLTypedBufferArray<float>> bufferDepthRange;
    std::shared_ptr<OGLFrameBuffer> frameBuffer;
    std::shared_ptr<OGLFrameBuffer> frameBufferHZB;

private:
    void _UpdateTextureSampler(Renderer::Impl& a_Rdr,
        const LightType& a_LightType,
        const LightShadowSettings& a_ShadowSettings,
        const size_t& a_ViewportCount);
    OGLCmdBuffer _cmdBuffer;
    OGLFence _executionFence { true };
};
}