#include <MSG/Renderer/OGL/RenderPasses/DfdSSAO.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdLight.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <Bindings.glsl>
#include <SSAO.glsl>

MSG::Renderer::DfdSSAO::DfdSSAO(Renderer::Impl& a_Renderer)
    : RenderPassInterface({ typeid(DfdOpaqueGeometry), typeid(DfdLight) })
    , ssaoBuffer(std::make_shared<OGLTypedBuffer<GLSL::SSAOSettings>>(a_Renderer.context))
{
}

void MSG::Renderer::DfdSSAO::UpdateSettings(Renderer::Impl& a_Renderer, const Renderer::RendererSettings& a_Settings)
{
    GLSL::SSAOSettings glslSSAOSettings = ssaoBuffer->Get();
    glslSSAOSettings.radius             = a_Settings.ssao.radius;
    glslSSAOSettings.strength           = a_Settings.ssao.strength;
    ssaoBuffer->Set(glslSSAOSettings);
    ssaoBuffer->Update();
}

void MSG::Renderer::DfdSSAO::Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses)
{
    renderPassInfo = a_RenderPasses.Get<DfdLight>().renderPassInfo;
    RenderPassInterface::Update(a_Renderer, a_RenderPasses);
    geometryFB = a_Renderer.renderPassesLibrary.Get<DfdOpaqueGeometry>().output;
}

void MSG::Renderer::DfdSSAO::Render(Impl& a_Renderer)
{
    auto& meshSubsystem                           = a_Renderer.subsystemsLibrary.Get<MeshSubsystem>();
    auto& activeScene                             = *a_Renderer.activeScene;
    auto& cmdBuffer                               = a_Renderer.renderCmdBuffer;
    const ShaderLibrary::ProgramKeywords keywords = { { "SSAO_QUALITY", std::to_string(int(a_Renderer.settings.ssao.quality) + 1) } };
    auto& shader                                  = *a_Renderer.shaderCache["DeferredSSAO"][keywords[0].second];
    if (!shader)
        shader = a_Renderer.shaderCompiler.CompileProgram("DeferredSSAO", keywords);
    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;
    OGLGraphicsPipelineInfo gpInfo;
    gpInfo.inputAssemblyState                      = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState                      = { .cullMode = GL_NONE };
    gpInfo.vertexInputState                        = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
    gpInfo.bindings                                = meshSubsystem.globalBindings;
    gpInfo.bindings.uniformBuffers[UBO_CAMERA + 1] = { .buffer = ssaoBuffer, .offset = 0, .size = ssaoBuffer->size };
    gpInfo.bindings.images[0]                      = { geometryFB->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER0].texture, GL_READ_ONLY, GL_RGBA32UI };
    gpInfo.bindings.images[1]                      = { geometryFB->info.colorBuffers[OUTPUT_FRAG_DFD_GBUFFER1].texture, GL_READ_ONLY, GL_RGBA32UI };
    gpInfo.colorBlend.attachmentStates.resize(1);
    gpInfo.colorBlend.attachmentStates[0].enableBlend         = true;
    gpInfo.colorBlend.attachmentStates[0].srcColorBlendFactor = GL_ZERO;
    gpInfo.colorBlend.attachmentStates[0].dstColorBlendFactor = GL_SRC_COLOR;
    gpInfo.colorBlend.attachmentStates[0].colorBlendOp        = GL_FUNC_ADD;
    gpInfo.colorBlend.attachmentStates[0].srcAlphaBlendFactor = GL_ZERO;
    gpInfo.colorBlend.attachmentStates[0].dstAlphaBlendFactor = GL_ONE;
    gpInfo.colorBlend.attachmentStates[0].alphaBlendOp        = GL_FUNC_ADD;
    gpInfo.shaderState.program                                = shader;
    gpInfo.depthStencilState.enableDepthTest                  = false;
    gpInfo.depthStencilState.enableStencilTest                = true;
    gpInfo.depthStencilState.front.compareOp                  = GL_EQUAL;
    gpInfo.depthStencilState.front.reference                  = 255;
    gpInfo.depthStencilState.back                             = gpInfo.depthStencilState.front;

    cmdBuffer.PushCmd<OGLCmdPushRenderPass>(renderPassInfo);
    cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
    cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
    cmdBuffer.PushCmd<OGLCmdEndRenderPass>();
}