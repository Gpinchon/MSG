#pragma once

#include <GL/glew.h>
#include <array>
#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace MSG {
class OGLBuffer;
class OGLTexture;
class OGLFrameBuffer;
class OGLProgram;
class OGLSampler;
class OGLVertexArray;
}

namespace MSG {
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
struct OGLInputAssemblyState {
    bool primitiveRestart    = false;
    GLenum primitiveTopology = GL_NONE;
};
struct OGLRasterizationState {
    bool rasterizerDiscardEnable  = false;
    bool depthClampEnable         = false;
    bool depthBiasEnable          = false;
    float depthBiasConstantFactor = 0;
    float depthBiasSlopeFactor    = 0;
    float depthBiasClamp          = 0;
    float lineWidth               = 1;
    GLenum polygonOffsetMode      = GL_POLYGON_OFFSET_FILL;
    GLenum polygonMode            = GL_FILL;
    GLenum cullMode               = GL_BACK;
    GLenum frontFace              = GL_CCW;
};
struct OGLVertexInputState {
    unsigned vertexCount = 0;
    unsigned indexCount  = 0;
    std::shared_ptr<OGLVertexArray> vertexArray;
};
struct OGLShaderState {
    std::shared_ptr<OGLProgram> program;
};
struct OGLColorBlendAttachmentState {
    unsigned index             = 0;
    bool enableBlend           = false;
    GLenum srcColorBlendFactor = GL_ONE;
    GLenum dstColorBlendFactor = GL_ZERO;
    GLenum colorBlendOp        = GL_FUNC_ADD;
    GLenum srcAlphaBlendFactor = GL_ONE;
    GLenum dstAlphaBlendFactor = GL_ZERO;
    GLenum alphaBlendOp        = GL_FUNC_ADD;
    glm::bvec4 colorWriteMask  = { true, true, true, true };
};
struct OGLColorBlendState {
    bool enableLogicOp       = false;
    GLenum logicOp           = GL_COPY;
    glm::vec4 blendConstants = { 0, 0, 0, 0 };
    std::vector<OGLColorBlendAttachmentState> attachmentStates;
};
struct OGLStencilOpState {
    GLenum failOp        = GL_KEEP; // the operation to be realized when stencil test FAILS
    GLenum depthFailOp   = GL_KEEP; // the operation to be realized when stencil test PASSES but depth test FAILS
    GLenum passOp        = GL_KEEP; // the operation to be realized when stencil & depth test PASSES
    GLenum compareOp     = GL_ALWAYS;
    uint32_t compareMask = ~0u; // a mask that is ANDed with ref and the buffer's content
    uint32_t writeMask   = ~0u; // a mask that is ANDed with the stencil value about to be written to the buffer
    uint32_t reference   = 0; // the reference value used in comparison.
};
struct OGLDepthStencilState {
    bool enableDepthTest       = true;
    bool enableDepthWrite      = true;
    bool enableDepthClamp      = false;
    bool enableDepthBoundsTest = false;
    bool enableStencilTest     = false;
    GLenum depthCompareOp      = GL_LESS;
    glm::dvec2 depthBounds     = { 0, 1 };
    glm::dvec2 depthRange      = { 0, 1 };
    OGLStencilOpState front    = {};
    OGLStencilOpState back     = {};
};
struct OGLBufferBindingInfo {
    std::shared_ptr<OGLBuffer> buffer = nullptr;
    uint32_t offset                   = 0;
    uint32_t size                     = 0;
};
struct OGLImageBindingInfo {
    std::shared_ptr<OGLTexture> texture = nullptr;
    GLenum access                       = GL_NONE;
    GLenum format                       = GL_NONE;
    int level                           = 0;
    int layer                           = 0;
    bool layered                        = false;
};
struct OGLTextureBindingInfo {
    std::shared_ptr<OGLTexture> texture = nullptr;
    std::shared_ptr<OGLSampler> sampler = nullptr;
};
struct OGLBindings {
    OGLBindings& operator+=(const OGLBindings& a_Other)
    {
        for (uint8_t index = 0; index < images.size(); index++) {
            auto& cur = images.at(index);
            auto& in  = a_Other.images.at(index);
            if (in.texture != nullptr) {
                assert(cur.texture == nullptr && "OGLBindings colliding");
                cur = in;
            }
        }
        for (uint8_t index = 0; index < textures.size(); index++) {
            auto& cur = textures.at(index);
            auto& in  = a_Other.textures.at(index);
            if (in.texture != nullptr) {
                assert(cur.texture == nullptr && "OGLBindings colliding");
                cur = in;
            }
        }
        for (uint8_t index = 0; index < uniformBuffers.size(); index++) {
            auto& cur = uniformBuffers.at(index);
            auto& in  = a_Other.uniformBuffers.at(index);
            if (in.buffer != nullptr) {
                assert(cur.buffer == nullptr && "OGLBindings colliding");
                cur = in;
            }
        }
        for (uint8_t index = 0; index < storageBuffers.size(); index++) {
            auto& cur = storageBuffers.at(index);
            auto& in  = a_Other.storageBuffers.at(index);
            if (in.buffer != nullptr) {
                assert(cur.buffer == nullptr && "OGLBindings colliding");
                cur = in;
            }
        }
        return *this;
    }
    std::array<OGLImageBindingInfo, 32> images;
    std::array<OGLTextureBindingInfo, 32> textures;
    std::array<OGLBufferBindingInfo, 32> uniformBuffers;
    std::array<OGLBufferBindingInfo, 32> storageBuffers;
};

struct OGLDrawCommand {
    OGLDrawCommand()
        : indexCount(0)
        , indexOffset(0)
    {
    }
    bool indexed            = false;
    uint32_t instanceCount  = 1;
    uint32_t instanceOffset = 0;
    uint32_t vertexOffset   = 0;
    union {
        struct {
            uint32_t indexCount;
            uint32_t indexOffset;
        };
        struct {
            uint32_t vertexCount;
        };
    };
};

struct OGLBasePipelineInfo {
    OGLBindings bindings; // the bindings for this Pipeline
    OGLShaderState shaderState; // the shader used to render the graphic pipeline
};

struct OGLGraphicsPipelineInfo : OGLBasePipelineInfo {
    OGLColorBlendState colorBlend;
    OGLDepthStencilState depthStencilState;
    OGLInputAssemblyState inputAssemblyState;
    OGLRasterizationState rasterizationState;
    OGLVertexInputState vertexInputState;
    std::vector<OGLDrawCommand> drawCommands;
};

struct OGLComputePipelineInfo : OGLBasePipelineInfo {
    uint16_t workgroupX  = 1;
    uint16_t workgroupY  = 1;
    uint16_t workgroupZ  = 1;
    GLenum memoryBarrier = GL_NONE;
};

using OGLPipelineInfo = std::variant<OGLGraphicsPipelineInfo, OGLComputePipelineInfo>;

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
    std::vector<OGLPipelineInfo> pipelines;
};
}
