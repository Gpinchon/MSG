#include <MSG/Renderer/OGL/RenderPasses/PassFog.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassLight.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassSSAO.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <Bindings.glsl>

MSG::Renderer::PassFog::PassFog(Renderer::Impl& a_Renderer)
    : RenderPassInterface({ typeid(PassOpaqueGeometry), typeid(PassLight), typeid(PassSSAO) })
{
}

void MSG::Renderer::PassFog::Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses)
{
    geometryFB     = a_Renderer.renderPassesLibrary.Get<PassOpaqueGeometry>().output;
    renderPassInfo = a_RenderPasses.Get<PassLight>().renderPassInfo;
    RenderPassInterface::Update(a_Renderer, a_RenderPasses);
}

void MSG::Renderer::PassFog::Render(Impl& a_Renderer)
{
    auto& meshSubsystem = a_Renderer.subsystemsLibrary.Get<MeshSubsystem>();
    auto& activeScene   = *a_Renderer.activeScene;
    auto& cmdBuffer     = a_Renderer.renderCmdBuffer;
    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;
    auto& shader        = *a_Renderer.shaderCache["DeferredFog"];
    if (!shader)
        shader = a_Renderer.shaderCompiler.CompileProgram("DeferredFog");
    OGLGraphicsPipelineInfo gpInfo;
    gpInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState = { .cullMode = GL_NONE };
    gpInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
    gpInfo.bindings           = meshSubsystem.globalBindings;
    gpInfo.bindings.images[0] = { geometryFB->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER0].texture, GL_READ_WRITE, GL_RGBA32UI };
    gpInfo.bindings.images[1] = { geometryFB->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER1].texture, GL_READ_WRITE, GL_RGBA32UI };
    gpInfo.colorBlend.attachmentStates.resize(1);
    gpInfo.colorBlend.attachmentStates[0].enableBlend         = true;
    gpInfo.colorBlend.attachmentStates[0].srcColorBlendFactor = GL_ONE;
    gpInfo.colorBlend.attachmentStates[0].dstColorBlendFactor = GL_SRC_ALPHA;
    gpInfo.colorBlend.attachmentStates[0].colorBlendOp        = GL_FUNC_ADD;
    gpInfo.colorBlend.attachmentStates[0].srcAlphaBlendFactor = GL_ZERO;
    gpInfo.colorBlend.attachmentStates[0].dstAlphaBlendFactor = GL_ONE;
    gpInfo.colorBlend.attachmentStates[0].alphaBlendOp        = GL_FUNC_ADD;
    gpInfo.shaderState.program                                = shader;

    cmdBuffer.PushCmd<OGLCmdPushRenderPass>(renderPassInfo);
    cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
    cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
}