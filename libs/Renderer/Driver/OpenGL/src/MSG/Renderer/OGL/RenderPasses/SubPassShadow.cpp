#include <MSG/Renderer/OGL/RenderPasses/SubPassShadow.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLPipelineInfo.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/SubPassVTFS.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <Bindings.glsl>

MSG::Renderer::SubPassShadow::SubPassShadow()
    : RenderSubPassInterface({ typeid(SubPassVTFS) })
{
}

void MSG::Renderer::SubPassShadow::Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass)
{
    geometryFB = a_Renderer.renderPassesLibrary.Get<PassOpaqueGeometry>().output;
}

void MSG::Renderer::SubPassShadow::UpdateSettings(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings)
{
    const ShaderLibrary::ProgramKeywords keywords = { { "SHADOW_QUALITY", std::to_string(int(a_Settings.shadowQuality) + 1) } };
    shader                                        = *a_Renderer.shaderCache["DeferredShadows"][keywords[0].second];
    if (shader == nullptr)
        shader = a_Renderer.shaderCompiler.CompileProgram("DeferredShadows", keywords);
}

void MSG::Renderer::SubPassShadow::Render(Impl& a_Renderer)
{
    auto& meshSubsystem = a_Renderer.subsystemsLibrary.Get<MeshSubsystem>();
    auto& cmdBuffer     = a_Renderer.renderCmdBuffer;
    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;
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
    gpInfo.colorBlend.attachmentStates[0].dstColorBlendFactor = GL_ONE;
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
    cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
    cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
}