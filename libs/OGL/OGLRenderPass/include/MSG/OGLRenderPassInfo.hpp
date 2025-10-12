#pragma once

#include <array>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Msg {
class OGLFrameBuffer;
}

namespace Msg {
union OGLClearColorValue {
    constexpr OGLClearColorValue() noexcept = default;
    constexpr OGLClearColorValue(
        float a_R = 0,
        float a_G = 0,
        float a_B = 0,
        float a_A = 0) noexcept
    {
        float32[0] = a_R;
        float32[1] = a_G;
        float32[2] = a_B;
        float32[3] = a_A;
    }
    constexpr OGLClearColorValue(
        int32_t a_R = 0,
        int32_t a_G = 0,
        int32_t a_B = 0,
        int32_t a_A = 0) noexcept
    {
        int32[0] = a_R;
        int32[1] = a_G;
        int32[2] = a_B;
        int32[3] = a_A;
    }
    constexpr OGLClearColorValue(
        uint32_t a_R = 0,
        uint32_t a_G = 0,
        uint32_t a_B = 0,
        uint32_t a_A = 0) noexcept
    {
        uint32[0] = a_R;
        uint32[1] = a_G;
        uint32[2] = a_B;
        uint32[3] = a_A;
    }
    int32_t int32[4] { 0, 0, 0, 0 };
    uint32_t uint32[4];
    float float32[4];
};

struct OGLViewportState {
    glm::uvec2 viewport      = { 0, 0 };
    glm::ivec2 scissorOffset = { 0, 0 };
    glm::uvec2 scissorExtent = { 0, 0 };
};

struct OGLFrameBufferClearColor {
    uint32_t index; // the index of the color buffer
    OGLClearColorValue color { 0, 0, 0, 0 };
};

struct OGLFrameBufferClearState {
    std::vector<OGLFrameBufferClearColor> colors;
    std::optional<uint32_t> depthStencil;
    std::optional<float> depth;
    std::optional<int32_t> stencil;
};

struct OGLFrameBufferState {
    std::shared_ptr<OGLFrameBuffer> framebuffer;
    OGLFrameBufferClearState clear;
    std::vector<GLenum> drawBuffers;
};

struct OGLRenderPassInfo {
    std::string name;
    OGLViewportState viewportState;
    OGLFrameBufferState frameBufferState;
};
}
