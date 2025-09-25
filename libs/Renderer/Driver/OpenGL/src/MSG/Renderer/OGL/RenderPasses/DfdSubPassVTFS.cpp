#include <MSG/Renderer/OGL/RenderPasses/DfdSubPassVTFS.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLPipelineInfo.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <Bindings.glsl>

MSG::Renderer::DfdSubPassVTFS::DfdSubPassVTFS(Renderer::Impl& a_Renderer)
    : shader(a_Renderer.shaderCompiler.CompileProgram("DeferredVTFS"))
{
}

void MSG::Renderer::DfdSubPassVTFS::Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass)
{
    geometryFB = a_Renderer.renderPassesLibrary.Get<DfdOpaqueGeometry>().output;
}

void MSG::Renderer::DfdSubPassVTFS::Render(Impl& a_Renderer)
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
    gpInfo.shaderState.program                 = shader;
    gpInfo.depthStencilState.enableDepthTest   = false;
    gpInfo.depthStencilState.enableStencilTest = true;
    gpInfo.depthStencilState.front.compareOp   = GL_EQUAL;
    gpInfo.depthStencilState.front.reference   = 255;
    gpInfo.depthStencilState.back              = gpInfo.depthStencilState.front;
    cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
    cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
}