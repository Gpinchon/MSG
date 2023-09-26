#include <Renderer/OGL/Components/MeshData.hpp>
#include <Renderer/OGL/Primitive.hpp>
#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/IndexBuffer.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/ProgramPipeline.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RenderBuffer.hpp>
#include <Renderer/OGL/Renderer.hpp>

#include <Renderer/OGL/GLSL/TransformUBO.hpp>
#include <Renderer/OGL/GLSL/CameraUBO.hpp>

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
    vec3 position;                                      \n\
    vec3 scale;                                         \n\
    vec4 rotation;                                      \n\
    mat4 transform;                                     \n\
};                                                      \n\
layout(binding = 0) uniform CameraUBO {                 \n\
    //TransformUBO transform;                             \n\
    mat4 projection;                                    \n\
    mat4 view;                                          \n\
} u_Camera;                                             \n\
layout(location = 0) in vec3	in_Position;            \n\
void main() {                                           \n\
    gl_Position = u_Camera.projection * u_Camera.view * vec4(in_Position, 1); \n\
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
    context.PushResourceCreationCmd(
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

auto CreateForwardPassCameraUBO(Renderer::Impl& a_Renderer)
{
    UBOInfo ubo;
    ubo.index  = 0;
    ubo.buffer = RAII::MakeWrapper<RAII::Buffer>(a_Renderer.context, sizeof(CameraUBO), nullptr, GL_DYNAMIC_STORAGE_BIT);
    return ubo;
}

auto CreateForwardShader(Renderer::Impl& a_Renderer)
{
    ShaderState shaderState;
    shaderState.pipeline = RAII::MakeWrapper<RAII::ProgramPipeline>(a_Renderer.context);
    shaderState.program  = CompileForwardShaders(a_Renderer.shaderCompiler);
    shaderState.stages   = GL_VERTEX_SHADER | GL_FRAGMENT_SHADER;
    return shaderState;
}

void ExecutePass(RAII::Context& a_Context, RenderPassInfo& a_Pass)
{
    a_Context.PushRenderCmd(
        [pass = a_Pass] {
            auto renderGeometryDebugGroup = RAII::DebugGroup("Execute Pass : " + pass.name);
            std::vector<int> globalUBOsToUnbind;
            auto& fbState           = pass.frameBufferState;
            auto& graphicsPipelines = pass.graphicsPipelines;
            {
                auto clearFBDebugGroup = RAII::DebugGroup("Clear Framebuffer");
                for (auto& clearColor : fbState.clearColors) {
                    glClearNamedFramebufferfv(
                        *fbState.frameBuffer,
                        GL_COLOR, clearColor.index, &clearColor.color[0]);
                }
                if (fbState.clearDepth.has_value()) {
                    glClearNamedFramebufferfv(
                        *fbState.frameBuffer,
                        GL_DEPTH, 0, &fbState.clearDepth.value());
                }
                if (fbState.clearStencil.has_value()) {
                    glClearNamedFramebufferiv(
                        *fbState.frameBuffer,
                        GL_STENCIL, 0, &fbState.clearStencil.value());
                }
            }

            {
                auto applyStatesDebugGroup = RAII::DebugGroup("Apply States");
                glViewport(0, 0, pass.viewportState.viewport.x, pass.viewportState.viewport.y);
                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_TRUE);
                glDepthFunc(GL_LESS);
            }
            {
                auto clearFBDebugGroup = RAII::DebugGroup("Bind UBOs");
                globalUBOsToUnbind.reserve(1024);
                for (auto& ubo : pass.UBOs) {
                    glBindBufferBase(GL_UNIFORM_BUFFER, ubo.index, *ubo.buffer);
                    globalUBOsToUnbind.push_back(ubo.index);
                }
            }
            {
                auto renderGeometryDebugGroup = RAII::DebugGroup("Render Geometry");
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *pass.frameBufferState.frameBuffer);
                for (uint32_t index = 0; index < graphicsPipelines.size(); ++index) {
                    auto& graphicsPipelineInfo = graphicsPipelines.at(index);
                    auto lastPipeline          = index > 0 ? &graphicsPipelines.at(index - 1) : nullptr;
                    for (auto& ubo : graphicsPipelineInfo.UBOs) {
                        glBindBufferBase(GL_UNIFORM_BUFFER, ubo.index, *ubo.buffer);
                    }
                    if (lastPipeline == nullptr
                        || lastPipeline->vertexInputState.vertexArray != graphicsPipelineInfo.vertexInputState.vertexArray) {
                        glBindVertexArray(*graphicsPipelineInfo.vertexInputState.vertexArray);
                    }
                    if (lastPipeline == nullptr
                        || lastPipeline->shaderState.program != graphicsPipelineInfo.shaderState.program) {
                        glUseProgram(*graphicsPipelineInfo.shaderState.program);
                    }
                    if (graphicsPipelineInfo.vertexInputState.indexBuffer != nullptr) {
                        glDrawElements(
                            graphicsPipelineInfo.rasterizationState.drawingMode,
                            graphicsPipelineInfo.vertexInputState.indexBuffer->indexCount,
                            graphicsPipelineInfo.vertexInputState.indexBuffer->indexFormat,
                            nullptr);
                    } else {
                        glDrawArrays(
                            graphicsPipelineInfo.rasterizationState.drawingMode,
                            0, graphicsPipelineInfo.vertexInputState.vertexBuffer->vertexCount);
                    }
                }
            }
            {
                auto clearStatesDebugGroup = RAII::DebugGroup("Clear states");
                glUseProgram(0);
                glBindVertexArray(0);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                for (auto& i : globalUBOsToUnbind)
                    glBindBufferBase(GL_UNIFORM_BUFFER, i, 0);
            }
            
        });
}

Impl::Impl(const CreateRendererInfo& a_Info)
    : name(a_Info.name)
    , version(a_Info.applicationVersion)
    , shaderCompiler(context)
    , forwardFrameBuffer(CreateForwardFrameBuffer(*this, 2048, 2048))
    , forwardShader(CreateForwardShader(*this))
    , forwardcameraUBO(CreateForwardPassCameraUBO(*this))
{
}

void Impl::Render()
{
    // return quietly
    if (activeScene == nullptr || activeRenderBuffer == nullptr)
        return;
    for (auto& renderPass : renderPasses) {
        ExecutePass(context, std::move(renderPass));
    }
    context.PushRenderCmd(
        [this] {
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
    context.ExecuteRenderCmds();
}

void Impl::Update()
{
    // return quietly
    if (activeScene == nullptr || activeRenderBuffer == nullptr)
        return;
    auto& renderBuffer = *activeRenderBuffer;
    // Update forward pass
    if (forwardFrameBuffer.frameBuffer == nullptr
        || forwardFrameBuffer.frameBuffer->width < renderBuffer->width
        || forwardFrameBuffer.frameBuffer->height < renderBuffer->height) {
        // Recreate framebuffer
        auto newWidth      = std::max(forwardFrameBuffer.frameBuffer->width, renderBuffer->width);
        auto newHeight     = std::max(forwardFrameBuffer.frameBuffer->height, renderBuffer->height);
        forwardFrameBuffer = CreateForwardFrameBuffer(*this, newWidth, newHeight);
    }
    RenderPassInfo forwardRenderPass;
    forwardRenderPass.name                        = "Forward";
    forwardRenderPass.UBOs                        = { forwardcameraUBO };
    forwardRenderPass.frameBufferState            = forwardFrameBuffer;
    forwardRenderPass.viewportState.viewport      = { renderBuffer->width, renderBuffer->height };
    forwardRenderPass.viewportState.scissorExtent = forwardRenderPass.viewportState.viewport;
    forwardRenderPass.graphicsPipelines.clear();
    auto view = activeScene->GetRegistry()->GetView<Component::MeshData, SG::Component::Transform>();
    for (auto& [entityID, meshData, transform] : view) {
        auto entityRef = activeScene->GetRegistry()->GetEntityRef(entityID);
        for (auto& primitive : meshData.primitives) {
            auto& graphicsPipelineInfo = forwardRenderPass.graphicsPipelines.emplace_back();
            graphicsPipelineInfo.UBOs  = { { 1, meshData.transformUBO } };
            graphicsPipelineInfo.shaderState = forwardShader;
            primitive->FillGraphicsPipelineInfo(graphicsPipelineInfo);
        }
        context.PushRenderCmd(
            [entityRef, UBO = meshData.transformUBO, transform = transform] {
                TransformUBO transformUBO = {};
                transformUBO.position     = transform.position;
                transformUBO.scale        = transform.scale;
                transformUBO.rotation     = transform.rotation;
                transformUBO.transform    = SG::Node::GetWorldTransformMatrix(entityRef);
                glNamedBufferSubData(*UBO, 0, sizeof(TransformUBO), &transformUBO);
            });
    }
    context.PushRenderCmd(
        [cameraUBO = forwardRenderPass.UBOs.front(), cameraEntity = activeScene->GetCamera()]() {
            CameraUBO cameraUBOData  = {};
            cameraUBOData.projection = cameraEntity.GetComponent<SG::Component::Camera>().projection.GetMatrix();
            cameraUBOData.view       = glm::inverse(SG::Node::GetWorldTransformMatrix(cameraEntity));
            glNamedBufferSubData(*cameraUBO.buffer, 0, sizeof(CameraUBO), &cameraUBOData);
        });
    renderPasses.clear();
    renderPasses.push_back(forwardRenderPass);
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
    a_Renderer->context.ExecuteResourceCreationCmds(true);
}

void Load(
    const Handle& a_Renderer,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (a_Entity.HasComponent<SG::Component::Mesh>()) {
        auto& mesh = a_Entity.GetComponent<SG::Component::Mesh>();
        a_Entity.AddComponent<Component::MeshData>(a_Renderer, mesh);
    }
    a_Renderer->context.ExecuteResourceCreationCmds(true);
}

void Unload(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene)
{
    auto& renderer = *a_Renderer;
    // wait for rendering to be done
    renderer.context.ExecuteRenderCmds(true);
    auto& registry = a_Scene.GetRegistry();
    auto view      = registry->GetView<SG::Component::Mesh, Component::MeshData>();
    for (auto& [entityID, mesh, meshData] : view) {
        registry->RemoveComponent<Component::MeshData>(entityID);
        for (auto& [primitive, material] : mesh.primitives) {
            if (renderer.primitives.at(primitive.get()).use_count() == 1)
                renderer.primitives.erase(primitive.get());
        }
    }
    renderer.context.ExecuteResourceDestructionCmds(true);
}

void Unload(
    const Handle& a_Renderer,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    auto& renderer = *a_Renderer;
    // wait for rendering to be done
    renderer.context.ExecuteRenderCmds(true);
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
