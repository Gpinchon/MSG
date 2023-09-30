#include <Renderer/OGL/Components/MeshData.hpp>
#include <Renderer/OGL/Primitive.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/ProgramPipeline.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RenderBuffer.hpp>
#include <Renderer/OGL/Renderer.hpp>

#include <Renderer/OGL/GLSL/CameraUBO.hpp>
#include <Renderer/OGL/GLSL/TransformUBO.hpp>

#include <Renderer/Structs.hpp>
#include <SG/Component/Camera.hpp>
#include <SG/Component/Mesh.hpp>
#include <SG/Entity/Node.hpp>
#include <SG/Scene/Scene.hpp>

#ifdef _WIN32
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/Window.hpp>
#endif

#include <GL/wglew.h>

#include <stdexcept>

namespace TabGraph::Renderer {
auto s_ForwardVertexCode = "                            \n\
#version 450                                            \n\
out gl_PerVertex                                        \n\
{                                                       \n\
    vec4 gl_Position;                                   \n\
};                                                      \n\
struct TransformUBO {                                   \n\
    //vec3 position;                                      \n\
    //vec3 scale;                                         \n\
    //vec4 rotation;                                      \n\
    mat4 matrix;                                     \n\
};                                                      \n\
layout(binding = 0) uniform CameraBlock {                 \n\
    //TransformUBO transform;                             \n\
    mat4 projection;                                    \n\
    mat4 view;                                          \n\
} u_Camera;                                             \n\
layout (binding = 1) uniform TransformBlock {           \n\
    TransformUBO u_ModelTransform;                      \n\
};                                                      \n\
layout(location = 0) in vec3	in_Position;            \n\
void main() {                                           \n\
    vec4 worldPos = u_ModelTransform.matrix * vec4(in_Position, 1);\n\
    gl_Position = u_Camera.projection * u_Camera.view * worldPos; \n\
}                                                       \n\
";

auto s_ForwardFragmentCode = "              \n\
void main() {                               \n\
    gl_FragColor = vec4(1);                 \n\
}                                           \n\
";

auto CompileForwardShaders(ShaderCompiler& a_ShaderCompiler)
{
    std::vector<RAII::Shader*> forwardShaders;
    forwardShaders.push_back(&a_ShaderCompiler.CompileShader(GL_VERTEX_SHADER, s_ForwardVertexCode));
    forwardShaders.push_back(&a_ShaderCompiler.CompileShader(GL_FRAGMENT_SHADER, s_ForwardFragmentCode));
    return RAII::MakeWrapper<RAII::Program>(a_ShaderCompiler.context, forwardShaders);
}

auto CreateForwardFrameBuffer(Renderer::Impl& a_Renderer, uint32_t a_Width, uint32_t a_Height)
{
    auto& context = a_Renderer.context;
    FrameBufferState frameBufferState;
    frameBufferState.frameBuffer = RAII::MakeWrapper<RAII::FrameBuffer>(context, a_Width, a_Height);
    frameBufferState.colorBuffers.push_back(
        RAII::MakeWrapper<RAII::Texture2D>(
            context, a_Width, a_Height, 1, GL_RGB8));
    frameBufferState.depthBuffer = RAII::MakeWrapper<RAII::Texture2D>(
        context, a_Width, a_Height, 1, GL_DEPTH_COMPONENT24);
    frameBufferState.clearColors = { { 0, { 1, 0, 0, 1 } } };
    frameBufferState.clearDepth  = 1;
    context.PushCmd(
        [frameBufferState = frameBufferState] {
            glNamedFramebufferTexture(
                *frameBufferState.frameBuffer,
                GL_COLOR_ATTACHMENT0, *frameBufferState.colorBuffers.front(), 0);
            glNamedFramebufferTexture(
                *frameBufferState.frameBuffer,
                GL_DEPTH_ATTACHMENT, *frameBufferState.depthBuffer, 0);
            constexpr std::array<GLenum, 1> drawArrays { GL_COLOR_ATTACHMENT0 };
            glNamedFramebufferDrawBuffers(
                *frameBufferState.frameBuffer,
                1, drawArrays.data());
        });
    return frameBufferState;
}

auto CreateForwardShader(Renderer::Impl& a_Renderer)
{
    ShaderState shaderState;
    shaderState.pipeline = RAII::MakeWrapper<RAII::ProgramPipeline>(a_Renderer.context);
    shaderState.program  = CompileForwardShaders(a_Renderer.shaderCompiler);
    shaderState.stages   = GL_VERTEX_SHADER | GL_FRAGMENT_SHADER;
    return shaderState;
}

Impl::Impl(const CreateRendererInfo& a_Info)
    : name(a_Info.name)
    , version(a_Info.applicationVersion)
    , shaderCompiler(context)
    , forwardFrameBuffer(CreateForwardFrameBuffer(*this, 2048, 2048))
    , forwardShader(CreateForwardShader(*this))
    , forwardcameraUBO(RAII::MakeWrapper<RAII::Buffer>(context, sizeof(CameraUBO), nullptr, GL_DYNAMIC_STORAGE_BIT))
{
}

void Impl::Render()
{
    // return quietly
    if (activeScene == nullptr || activeRenderBuffer == nullptr) {
        return;
    }
    context.PushCmd(
        [
            renderPasses = renderPasses,
            activeRenderBuffer = activeRenderBuffer,
            forwardFrameBuffer = forwardFrameBuffer
        ] () {
            for (auto& renderPass : renderPasses) {
                ExecuteRenderPass(renderPass);
            }
            auto& dstImage             = *activeRenderBuffer;
            auto& srcImage             = forwardFrameBuffer.colorBuffers.front();
            auto renderSceneDebugGroup = RAII::DebugGroup("Copy result to buffer");
            glCopyImageSubData(
                *srcImage, GL_TEXTURE_2D, 0,
                0, 0, 0,
                *dstImage, GL_TEXTURE_2D, 0,
                0, 0, 0,
                dstImage->width, dstImage->height, 1);
        });
    context.ExecuteCmds(context.Busy());
}

struct UniformBufferUpdate {
    RAII::Wrapper<RAII::Buffer> buffer;
    std::vector<std::byte> data;
    uint32_t offset = 0;
};

void Impl::Update()
{
    // return quietly
    if (activeScene == nullptr || activeRenderBuffer == nullptr)
        return;
    auto& renderBuffer = *activeRenderBuffer;
    // Update forward pass
    if (forwardFrameBuffer.frameBuffer->width < renderBuffer->width
        || forwardFrameBuffer.frameBuffer->height < renderBuffer->height) {
        // Recreate framebuffer
        auto newWidth      = std::max(forwardFrameBuffer.frameBuffer->width, renderBuffer->width);
        auto newHeight     = std::max(forwardFrameBuffer.frameBuffer->height, renderBuffer->height);
        forwardFrameBuffer = CreateForwardFrameBuffer(*this, newWidth, newHeight);
    }
    RenderPassInfo forwardRenderPass;
    forwardRenderPass.name                        = "Forward";
    forwardRenderPass.UBOs                        = { { 0, forwardcameraUBO } };
    forwardRenderPass.frameBufferState            = forwardFrameBuffer;
    forwardRenderPass.viewportState.viewport      = { renderBuffer->width, renderBuffer->height };
    forwardRenderPass.viewportState.scissorExtent = forwardRenderPass.viewportState.viewport;
    forwardRenderPass.graphicsPipelines.clear();
    std::vector<UniformBufferUpdate> uboUpdates;
    auto view = activeScene->GetRegistry()->GetView<Component::MeshData, SG::Component::Transform>();
    for (auto& [entityID, meshData, transform] : view) {
        auto entityRef = activeScene->GetRegistry()->GetEntityRef(entityID);
        for (auto& primitive : meshData.primitives) {
            auto& graphicsPipelineInfo       = forwardRenderPass.graphicsPipelines.emplace_back();
            graphicsPipelineInfo.UBOs        = { { 1, meshData.transformUBO } };
            graphicsPipelineInfo.shaderState = forwardShader;
            primitive->FillGraphicsPipelineInfo(graphicsPipelineInfo);
        }
        TransformUBO transformUBO {};
        UniformBufferUpdate uboUpdate;
        transformUBO.matrix = SG::Node::GetWorldTransformMatrix(entityRef);
        uboUpdate.buffer = meshData.transformUBO;
        uboUpdate.data      = { (std::byte*)&transformUBO, ((std::byte*)&transformUBO) + sizeof(transformUBO) };
        uboUpdates.push_back(std::move(uboUpdate));
    }
    {
        UniformBufferUpdate uboUpdate;
        CameraUBO cameraUBOData {};
        cameraUBOData.projection = activeScene->GetCamera().GetComponent<SG::Component::Camera>().projection.GetMatrix();
        cameraUBOData.view       = glm::inverse(SG::Node::GetWorldTransformMatrix(activeScene->GetCamera()));
        uboUpdate.buffer         = forwardRenderPass.UBOs.front().buffer;
        uboUpdate.data           = { (std::byte*)&cameraUBOData, ((std::byte*)&cameraUBOData) + sizeof(cameraUBOData) };
        uboUpdates.push_back(std::move(uboUpdate));
    }
    context.PushCmd([uboUpdates = std::move(uboUpdates)] {
        for (auto& uboUpdate : uboUpdates) {
            glNamedBufferSubData(
                *uboUpdate.buffer,
                uboUpdate.offset,
                uboUpdate.data.size(), uboUpdate.data.data());
        }
    });
    renderPasses.clear();
    renderPasses.push_back(forwardRenderPass);
    context.ExecuteCmds(context.Busy());
}

Handle Create(const CreateRendererInfo& a_Info)
{
    return Handle(new Impl(a_Info));
}

void SetActiveRenderBuffer(const Handle& a_Renderer, const RenderBuffer::Handle& a_RenderBuffer)
{
    a_Renderer->activeRenderBuffer = a_RenderBuffer;
}

RenderBuffer::Handle GetActiveRenderBuffer(const Handle& a_Renderer)
{
    return a_Renderer->activeRenderBuffer;
}

void SetActiveScene(const Handle& a_Renderer, SG::Scene* const a_Scene)
{
    a_Renderer->activeScene = a_Scene;
}

SG::Scene* GetActiveScene(const Handle& a_Renderer)
{
    return a_Renderer->activeScene;
}

void Load(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene)
{
    auto& registry = a_Scene.GetRegistry();
    auto view      = registry->GetView<SG::Component::Mesh>(ECS::Exclude<Component::MeshData> {});
    for (auto& [entityID, mesh] : view) {
        registry->AddComponent<Component::MeshData>(entityID, a_Renderer, mesh);
    }
    a_Renderer->context.ExecuteCmds();
}

void Load(
    const Handle& a_Renderer,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (a_Entity.HasComponent<SG::Component::Mesh>()) {
        auto& mesh = a_Entity.GetComponent<SG::Component::Mesh>();
        a_Entity.AddComponent<Component::MeshData>(a_Renderer, mesh);
    }
    a_Renderer->context.ExecuteCmds();
}

void Unload(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene)
{
    auto& renderer = *a_Renderer;
    // wait for rendering to be done
    auto& registry = a_Scene.GetRegistry();
    auto view      = registry->GetView<SG::Component::Mesh, Component::MeshData>();
    for (auto& [entityID, mesh, meshData] : view) {
        registry->RemoveComponent<Component::MeshData>(entityID);
        for (auto& [primitive, material] : mesh.primitives) {
            if (renderer.primitives.at(primitive.get()).use_count() == 1)
                renderer.primitives.erase(primitive.get());
        }
    }
}

void Unload(
    const Handle& a_Renderer,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    auto& renderer = *a_Renderer;
    // wait for rendering to be done
    if (a_Entity.HasComponent<Component::MeshData>())
        a_Entity.RemoveComponent<Component::MeshData>();
}

void Render(
    const Handle& a_Renderer)
{
    a_Renderer->Render();
}

void Update(const Handle& a_Renderer)
{
    a_Renderer->Update();
}
}
