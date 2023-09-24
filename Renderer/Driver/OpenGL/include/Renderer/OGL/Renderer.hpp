#pragma once

#include <Renderer/Handles.hpp>

#include <Renderer/OGL/RAII/Wrapper.hpp>

#ifdef WIN32
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Window.hpp>
#endif

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <unordered_map>

namespace TabGraph::SG {
struct Primitive;
}

namespace TabGraph::Renderer {
struct Primitive;
struct CreateRendererInfo;
}

namespace TabGraph::Renderer::RAII {
struct FrameBuffer;
struct VertexBuffer;
struct VertexArray;
struct IndexBuffer;
struct Texture2D;
struct Program;
struct Shader;
}

namespace TabGraph::Renderer {
struct ViewportState {
    glm::uvec2 viewport;
    glm::ivec2 scissorOffset;
    glm::uvec2 scissorExtent;
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
    RAII::Wrapper<RAII::Program> program;
    uint32_t stages = 0; // stages to use within this program
};
struct GraphicsPipelineInfo {
    RasterizationState rasterizationState;
    VertexInputState vertexInputState;
    ShaderState shaderState;
};
struct RenderPassInfo {
    ViewportState viewportState;
    uint32_t clear;
    RAII::Wrapper<RAII::FrameBuffer> frameBuffer;
    RAII::Wrapper<RAII::Texture2D> depthBuffer;
    std::vector<RAII::Wrapper<RAII::Texture2D>> colorBuffers;
    std::vector<uint32_t> drawBuffers;
    std::vector<GraphicsPipelineInfo> graphicsPipelines;
};
}

namespace TabGraph::Renderer {
struct Impl {
    Impl(const CreateRendererInfo& a_Info);
    void CreateForwardRenderBuffer(uint32_t a_Width, uint32_t a_Height);
    RAII::Window window { "DummyWindow", "DummyWindow" };
    RAII::Context context { window.hwnd, true };
    RenderPassInfo forwardRenderPass;
    uint32_t version;
    std::string name;
    std::unordered_map<SG::Primitive*, std::shared_ptr<Primitive>> primitives;
    std::unordered_map<std::string, RAII::Wrapper<RAII::Shader>> shaderCache;
};
}
