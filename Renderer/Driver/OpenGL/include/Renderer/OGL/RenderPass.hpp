#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <glm/glm.hpp>
#include <optional>

namespace TabGraph::Renderer::RAII {
struct Buffer;
struct FrameBuffer;
struct IndexBuffer;
struct Program;
struct ProgramPipeline;
struct Shader;
struct Texture2D;
struct VertexBuffer;
struct VertexArray;
}

namespace TabGraph::Renderer {
struct ViewportState {
    glm::uvec2 viewport      = { 0, 0 };
    glm::ivec2 scissorOffset = { 0, 0 };
    glm::uvec2 scissorExtent = { 0, 0 };
};
struct RasterizationState {
    uint32_t drawingMode = 0;
};
struct VertexInputState {
    RAII::Wrapper<RAII::VertexArray> vertexArray;
    RAII::Wrapper<RAII::VertexBuffer> vertexBuffer;
    RAII::Wrapper<RAII::IndexBuffer> indexBuffer;
};
struct ShaderState {
    RAII::Wrapper<RAII::ProgramPipeline> pipeline;
    RAII::Wrapper<RAII::Program> program;
    uint32_t stages = 0; // stages to use within this program
};
struct UBOInfo {
    uint32_t index = 0; // layout(binding = ?)
    RAII::Wrapper<RAII::Buffer> buffer;
};

struct GraphicsPipelineInfo {
    ShaderState shaderState; // the shader used to render the graphic pipeline
    RasterizationState rasterizationState;
    VertexInputState vertexInputState;
    std::vector<UBOInfo> UBOs; // UBOs that'll be updated for each GraphicsPipeline
};

struct FrameBufferClearColor {
    uint32_t index; // the index of the color buffer
    glm::vec4 color;
};
struct FrameBufferState {
    std::vector<FrameBufferClearColor> clearColors;
    std::optional<float> clearDepth;
    std::optional<int> clearStencil;
    RAII::Wrapper<RAII::FrameBuffer> frameBuffer;
    RAII::Wrapper<RAII::Texture2D> depthBuffer;
    std::vector<RAII::Wrapper<RAII::Texture2D>> colorBuffers;
    std::vector<uint32_t> drawBuffers;
};

struct RenderPassInfo {
    RenderPassInfo()
    {
        graphicsPipelines.reserve(1024);
    }
    std::string name;
    std::vector<UBOInfo> UBOs; // UBOs that'll be shared accross the whole pass
    ViewportState viewportState;
    FrameBufferState frameBufferState;
    std::vector<GraphicsPipelineInfo> graphicsPipelines;
};
}
