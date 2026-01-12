#pragma once

#include <MSG/Component.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/OGLTypedBuffer.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <memory>
#include <optional>
#include <variant>

namespace Msg {
struct LightShadowSettings;
class OGLBindlessTextureSampler;
class OGLFrameBuffer;
class OGLSampler;
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
        const std::shared_ptr<OGLSampler>& a_Sampler,
        const LightShadowSettings& a_ShadowSettings,
        const size_t& a_ViewportCount);
    void UpdateDepthRange();
    float minDepth = 0;
    float maxDepth = 1;
    std::shared_ptr<OGLBindlessTextureSampler> textureSampler;
    // used for shadow rendering
    std::shared_ptr<OGLTypedBufferArray<float>> bufferDepthRange;
    std::shared_ptr<OGLTypedBufferArray<float>> bufferDepthRange_Prev;
    std::shared_ptr<OGLFrameBuffer> frameBuffer;

private:
    void _UpdateTextureSampler(Renderer::Impl& a_Rdr,
        const std::shared_ptr<OGLSampler>& a_Sampler,
        const LightShadowSettings& a_ShadowSettings,
        const size_t& a_ViewportCount);
};
}