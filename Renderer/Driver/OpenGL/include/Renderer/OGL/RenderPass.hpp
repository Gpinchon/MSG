#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <optional>

namespace TabGraph::Renderer {
struct Context;
}

namespace TabGraph::Renderer::RAII {
struct Buffer;
struct FrameBuffer;
struct Program;
struct ProgramPipeline;
struct Sampler;
struct Shader;
struct Texture;
struct Texture2D;
struct VertexArray;
}

namespace TabGraph::Renderer {
union ClearColorValue {
    constexpr ClearColorValue() noexcept = default;
    constexpr ClearColorValue(
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
    constexpr ClearColorValue(
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
    constexpr ClearColorValue(
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
struct ViewportState {
    glm::uvec2 viewport      = { 0, 0 };
    glm::ivec2 scissorOffset = { 0, 0 };
    glm::uvec2 scissorExtent = { 0, 0 };
};
struct InputAssemblyState {
    bool primitiveRestart    = false;
    GLenum primitiveTopology = GL_NONE;
};
struct RasterizationState {
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
struct VertexInputState {
    unsigned vertexCount = 0;
    unsigned indexCount  = 0;
    std::shared_ptr<RAII::VertexArray> vertexArray;
};
struct ShaderState {
    std::shared_ptr<RAII::ProgramPipeline> pipeline;
    std::shared_ptr<RAII::Program> program;
    uint32_t stages = 0; // stages to use within this program
};
struct StencilOpState {
    GLenum failOp        = GL_KEEP; // the operation to be realized when stencil test FAILS
    GLenum depthFailOp   = GL_KEEP; // the operation to be realized when stencil test PASSES but depth test FAILS
    GLenum passOp        = GL_KEEP; // the operation to be realized when stencil & depth test PASSES
    GLenum compareOp     = GL_ALWAYS;
    uint32_t compareMask = ~0u; // a mask that is ANDed with ref and the buffer's content
    uint32_t writeMask   = ~0u; // a mask that is ANDed with the stencil value about to be written to the buffer
    uint32_t reference   = 0; // the reference value used in comparison.
};
struct DepthStencilState {
    bool enableDepthTest       = true;
    bool enableDepthWrite      = true;
    bool enableDepthBoundsTest = false;
    bool enableStencilTest     = false;
    GLenum depthCompareOp      = GL_LESS;
    glm::dvec2 depthBounds     = { 0, 1 };
    StencilOpState front       = {};
    StencilOpState back        = {};
};
struct BufferBindingInfo {
    GLenum target                        = 0; // GL_UNIFORM_BUFFER GL_SHADER_STORAGE_BUFFER...
    uint32_t index                       = 0; // layout(binding = ?)
    std::shared_ptr<RAII::Buffer> buffer = nullptr;
    uint32_t offset                      = 0;
    uint32_t size                        = 0;
};
struct ImageBindingInfo {
    uint bindingIndex = 0;
    std::shared_ptr<RAII::Texture> texture;
    int level     = 0;
    int layer     = 0;
    bool layered  = false;
    GLenum access = GL_NONE;
    GLenum format = GL_NONE;
};
struct TextureBindingInfo {
    uint bindingIndex = 0;
    GLenum target     = GL_NONE;
    std::shared_ptr<RAII::Texture> texture;
    std::shared_ptr<RAII::Sampler> sampler;
};
struct Bindings {
    std::vector<ImageBindingInfo> images;
    std::vector<TextureBindingInfo> textures;
    std::vector<BufferBindingInfo> buffers;
};

struct GraphicsPipelineInfo {
    DepthStencilState depthStencilState;
    ShaderState shaderState; // the shader used to render the graphic pipeline
    InputAssemblyState inputAssemblyState;
    RasterizationState rasterizationState;
    VertexInputState vertexInputState;
    Bindings bindings; // the bindings for this Graphics Pipeline
};

struct FrameBufferClearColor {
    uint32_t index; // the index of the color buffer
    ClearColorValue color { 0, 0, 0, 0 };
};
struct FrameBufferClearState {
    std::vector<FrameBufferClearColor> colors;
    std::optional<float> depth;
    std::optional<int> stencil;
};
struct FrameBufferState {
    std::shared_ptr<RAII::FrameBuffer> framebuffer;
    FrameBufferClearState clear;
    std::vector<GLenum> drawBuffers;
};

struct RenderPassInfo {
    RenderPassInfo()
    {
        graphicsPipelines.reserve(1024);
    }
    std::string name;
    Bindings bindings; // the bindings for the whole Render Pass
    ViewportState viewportState;
    FrameBufferState frameBufferState;
    std::vector<GraphicsPipelineInfo> graphicsPipelines;
};

struct RenderPass {
    RenderPass(const RenderPassInfo& a_Info);
    void Execute() const;
    const RenderPassInfo info;
};
}
