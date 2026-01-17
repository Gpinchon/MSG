#include <MSG/Renderer/OGL/RenderPasses/SubPassOITCompositing.hpp>

#include <MSG/Renderer/OGL/RenderPasses/PassOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/SubPassOITForward.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>
#include <MSG/Renderer/RenderPassInterface.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLPipelineInfo.hpp>
#include <MSG/OGLTexture3D.hpp>

#include <Bindings.glsl>
#include <OIT.glsl>

constexpr std::vector<Msg::OGLColorBlendAttachmentState> GetOITBlending()
{
    return {
        Msg::OGLColorBlendAttachmentState {
            .index               = 0,
            .enableBlend         = true,
            .srcColorBlendFactor = GL_ONE,
            .dstColorBlendFactor = GL_ONE_MINUS_SRC_ALPHA,
            .srcAlphaBlendFactor = GL_ONE,
            .dstAlphaBlendFactor = GL_ONE_MINUS_SRC_ALPHA },
    };
}

Msg::Renderer::SubPassOITCompositing::SubPassOITCompositing(Renderer::Impl& a_Renderer)
    : RenderSubPassInterface({ typeid(SubPassOITForward) })
    , shader(a_Renderer.shaderCompiler.CompileProgram("OITCompositing"))
{
}

void Msg::Renderer::SubPassOITCompositing::Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass)
{
    auto& oitForward = a_ParentPass->Get<SubPassOITForward>();
    velocity         = oitForward.velocity;
    gBuffer0         = oitForward.gBuffer0;
    gBuffer1         = oitForward.gBuffer1;
    depth            = oitForward.depth;
}

void Msg::Renderer::SubPassOITCompositing::Render(Impl& a_Renderer)
{
    auto& meshSubsystem = a_Renderer.subsystemsLibrary.Get<MeshSubsystem>();
    auto& geometryPass  = a_Renderer.renderPassesLibrary.Get<PassOpaqueGeometry>();
    auto& velocityBuf   = geometryPass.output->info.colorBuffers[OUTPUT_FRAG_VELOCITY].texture;
    auto& gDataBuf0     = geometryPass.output->info.colorBuffers[OUTPUT_FRAG_GBUFFER0].texture;
    auto& gDataBuf1     = geometryPass.output->info.colorBuffers[OUTPUT_FRAG_GBUFFER1].texture;
    auto& cmdBuffer     = a_Renderer.renderCmdBuffer;
    OGLGraphicsPipelineInfo gpInfo;
    gpInfo.colorBlend          = { .attachmentStates = GetOITBlending() };
    gpInfo.depthStencilState   = { .enableDepthTest = false };
    gpInfo.shaderState.program = shader;
    gpInfo.inputAssemblyState  = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState  = { .cullMode = GL_NONE };
    gpInfo.vertexInputState    = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };

    gpInfo.bindings                          = meshSubsystem.globalBindings;
    gpInfo.bindings.images[IMG_OIT_VELOCITY] = { .texture = velocity, .access = GL_READ_ONLY, .format = GL_RG16F, .layered = true };
    gpInfo.bindings.images[IMG_OIT_GBUFFER0] = { .texture = gBuffer0, .access = GL_READ_ONLY, .format = GL_RGBA32UI, .layered = true };
    gpInfo.bindings.images[IMG_OIT_GBUFFER1] = { .texture = gBuffer1, .access = GL_READ_ONLY, .format = GL_RGBA32UI, .layered = true };
    gpInfo.bindings.images[IMG_OIT_DEPTH]    = { .texture = depth, .access = GL_READ_ONLY, .format = GL_R32UI, .layered = true };

    gpInfo.bindings.images[IMG_OIT_OPAQUE_VELOCITY] = { .texture = velocityBuf, .access = GL_WRITE_ONLY, .format = GL_RG16F, .layered = false };
    gpInfo.bindings.images[IMG_OIT_OPAQUE_GBUFFER0] = { .texture = gDataBuf0, .access = GL_WRITE_ONLY, .format = GL_RGBA32UI, .layered = false };
    gpInfo.bindings.images[IMG_OIT_OPAQUE_GBUFFER1] = { .texture = gDataBuf1, .access = GL_WRITE_ONLY, .format = GL_RGBA32UI, .layered = false };

    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;
    cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
    cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    // cmdBuffer.PushCmd<OGLCmdMemoryBarrier>(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT, true);
}
