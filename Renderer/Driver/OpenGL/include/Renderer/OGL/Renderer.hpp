#pragma once

#include <Renderer/Handles.hpp>

#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/RenderPass.hpp>
#include <Renderer/OGL/ShaderCompiler.hpp>

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
struct Scene;
}

namespace TabGraph::Renderer {
struct Primitive;
struct CreateRendererInfo;
}

namespace TabGraph::Renderer {
struct Impl {
    Impl(const CreateRendererInfo& a_Info);
    void Render();
    void Update();
    RAII::Window window { "DummyWindow", "DummyWindow" };
    RAII::Context context { window.hwnd, true };

    uint32_t version;
    std::string name;
    std::unordered_map<SG::Primitive*, std::shared_ptr<Primitive>> primitives;
    ShaderCompiler shaderCompiler;

    RenderBuffer::Handle activeRenderBuffer = nullptr;
    SG::Scene* activeScene = nullptr;

    std::vector<RenderPassInfo> renderPasses;
    FrameBufferState forwardFrameBuffer;
    ShaderState forwardShader;
    UBOInfo forwardcameraUBO;
};
}
