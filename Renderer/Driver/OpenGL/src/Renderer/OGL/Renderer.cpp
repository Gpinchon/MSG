#include <Renderer/OGL/Components/MeshData.hpp>
#include <Renderer/OGL/Primitive.hpp>
#include <Renderer/OGL/RAII/DebugGroup.hpp>
#include <Renderer/OGL/RAII/FrameBuffer.hpp>
#include <Renderer/OGL/RAII/IndexBuffer.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RenderBuffer.hpp>
#include <Renderer/OGL/Renderer.hpp>

#include <Renderer/Structs.hpp>
#include <SG/Component/Mesh.hpp>
#include <SG/Scene/Scene.hpp>

#ifdef _WIN32
#include <Renderer/OGL/Win32/Context.hpp>
#include <Renderer/OGL/Win32/Error.hpp>
#include <Renderer/OGL/Win32/Window.hpp>
#endif

#include <GL/wglew.h>

#include <stdexcept>

namespace TabGraph::Renderer {
Handle Create(const CreateRendererInfo& a_Info)
{
    return Handle(new Impl(a_Info));
}

Impl::Impl(const CreateRendererInfo& a_Info)
    : name(a_Info.name)
    , version(a_Info.applicationVersion)
{
    forwardRenderPass.graphicsPipelines.reserve(1024);
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

void Impl::CreateForwardRenderBuffer(uint32_t a_Width, uint32_t a_Height)
{
    forwardRenderPass.frameBuffer = RAII::MakeWrapper<RAII::FrameBuffer>(context);
    forwardRenderPass.colorBuffers.push_back(
        RAII::MakeWrapper<RAII::Texture2D>(
            context, a_Width, a_Height, 1, GL_RGB8));
    forwardRenderPass.depthBuffer = RAII::MakeWrapper<RAII::Texture2D>(
        context, a_Width, a_Height, 1, GL_DEPTH_COMPONENT24);
    context.PushResourceCreationCmd(
        [this] {
            glNamedFramebufferTexture(
                *forwardRenderPass.frameBuffer,
                GL_COLOR_ATTACHMENT0, *forwardRenderPass.colorBuffers.front(), 0);
            glNamedFramebufferTexture(
                *forwardRenderPass.frameBuffer,
                GL_DEPTH_ATTACHMENT, *forwardRenderPass.depthBuffer, 0);
            constexpr std::array<GLenum, 1> drawArrays { GL_COLOR_ATTACHMENT0 };
            glNamedFramebufferDrawBuffers(
                *forwardRenderPass.frameBuffer,
                1, drawArrays.data());
        });
}

void Render(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene,
    const RenderBuffer::Handle& a_Buffer)
{
    auto& renderer     = *a_Renderer;
    auto& context      = renderer.context;
    auto& renderBuffer = *a_Buffer;

    // Update forward pass
    auto& forwardRenderPass = renderer.forwardRenderPass;
    if (forwardRenderPass.frameBuffer == nullptr
        || forwardRenderPass.frameBuffer->width < renderBuffer->width
        || forwardRenderPass.frameBuffer->height < renderBuffer->height) {
        // Recreate framebuffer
        renderer.CreateForwardRenderBuffer(renderBuffer->width, renderBuffer->height);
    }
    renderer.forwardRenderPass.viewportState.viewport      = { renderBuffer->width, renderBuffer->height };
    renderer.forwardRenderPass.viewportState.scissorExtent = renderer.forwardRenderPass.viewportState.viewport;

    renderer.forwardRenderPass.graphicsPipelines.clear();
    auto view = a_Scene.GetRegistry()->GetView<Component::MeshData>();
    for (auto& [entityID, meshData] : view) {
        for (auto& primitive : meshData.primitives) {
            auto& graphicsPipelineInfo = renderer.forwardRenderPass.graphicsPipelines.emplace_back();
            primitive->FillGraphicsPipelineInfo(graphicsPipelineInfo);
        }
    }

    renderer.context.PushRenderCmd(
        [&renderer, &a_Buffer] {
            auto renderSceneDebugGroup = RAII::DebugGroup("Clear back buffer");
            float color[4]             = { 1, 0, 0, 1 };
            glClearTexImage(
                **a_Buffer,
                0, // level
                GL_RGBA,
                GL_FLOAT,
                color);
        });
    renderer.context.PushRenderCmd(
        [&renderer] {
            auto renderGeometryDebugGroup = RAII::DebugGroup("Forward Pass");
            auto& graphicsPipelines       = renderer.forwardRenderPass.graphicsPipelines;
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, *renderer.forwardRenderPass.frameBuffer);
            {
                auto clearFBDebugGroup = RAII::DebugGroup("Clear Framebuffer");
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            for (uint32_t index = 0; index < graphicsPipelines.size(); ++index) {
                auto& graphicsPipelineInfo = graphicsPipelines.at(index);
                auto lastPipeline          = index > 0 ? &graphicsPipelines.at(index - 1) : nullptr;
                if (lastPipeline == nullptr
                    || lastPipeline->vertexInputState.vertexArray != graphicsPipelineInfo.vertexInputState.vertexArray) {
                    glBindVertexArray(*graphicsPipelineInfo.vertexInputState.vertexArray);
                }
                if (graphicsPipelineInfo.vertexInputState.indexBuffer != nullptr) {
                    if (lastPipeline == nullptr
                        || lastPipeline->vertexInputState.indexBuffer != graphicsPipelineInfo.vertexInputState.indexBuffer) {
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *graphicsPipelineInfo.vertexInputState.indexBuffer);
                    }
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
            glBindVertexArray(0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        });
    renderer.context.ExecuteRenderCmds();
}

void Update(const Handle& a_Renderer)
{
}
}
