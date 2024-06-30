#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <optional>

namespace TabGraph::Renderer::RAII {
struct Buffer;
struct FrameBuffer;
struct Program;
struct ProgramPipeline;
struct Shader;
struct Texture2D;
struct VertexArray;
}

namespace TabGraph::Renderer {
struct ViewportState {
    glm::uvec2 viewport      = { 0, 0 };
    glm::ivec2 scissorOffset = { 0, 0 };
    glm::uvec2 scissorExtent = { 0, 0 };
};
struct RasterizationState {
    GLenum drawingMode = 0;
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
struct BufferBindingInfo {
    uint32_t target = 0; // GL_UNIFORM_BUFFER GL_SHADER_STORAGE_BUFFER...
    uint32_t index  = 0; // layout(binding = ?)
    std::shared_ptr<RAII::Buffer> buffer;
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

struct GraphicsPipelineInfo {
    DepthStencilState depthStencilState;
    ShaderState shaderState; // the shader used to render the graphic pipeline
    RasterizationState rasterizationState;
    VertexInputState vertexInputState;
    std::vector<BufferBindingInfo> buffers; // buffers that'll be updated for each GraphicsPipeline
};

struct FrameBufferClearColor {
    uint32_t index; // the index of the color buffer
    glm::vec4 color;
};
struct FrameBufferState {
    std::vector<FrameBufferClearColor> clearColors;
    std::optional<float> clearDepth;
    std::optional<int> clearStencil;
    std::shared_ptr<RAII::FrameBuffer> frameBuffer;
    std::shared_ptr<RAII::Texture2D> depthBuffer;
    std::vector<std::shared_ptr<RAII::Texture2D>> colorBuffers;
    std::vector<GLenum> drawBuffers;
};

struct RenderPassInfo {
    RenderPassInfo()
    {
        graphicsPipelines.reserve(1024);
    }
    std::string name;
    std::vector<BufferBindingInfo> buffers; // buffers that'll be shared accross the whole pass
    ViewportState viewportState;
    FrameBufferState frameBufferState;
    std::vector<GraphicsPipelineInfo> graphicsPipelines;
};

void ExecuteRenderPass(const RenderPassInfo& a_Pass);
}
