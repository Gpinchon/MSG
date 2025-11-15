#include <MSG/Renderer/OGL/RenderPasses/SubPassOITCompositing.hpp>

#include <MSG/Renderer/OGL/RenderPasses/SubPassOITForward.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/RenderPassInterface.hpp>

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
        Msg::OGLColorBlendAttachmentState {
            .index               = 1,
            .enableBlend         = true,
            .srcColorBlendFactor = GL_SRC_ALPHA,
            .dstColorBlendFactor = GL_ONE_MINUS_SRC_ALPHA,
            .srcAlphaBlendFactor = GL_ONE,
            .dstAlphaBlendFactor = GL_ONE },
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
    color            = oitForward.color;
    velocity         = oitForward.velocity;
    depth            = oitForward.depth;
}

void Msg::Renderer::SubPassOITCompositing::Render(Impl& a_Renderer)
{
    auto& cmdBuffer = a_Renderer.renderCmdBuffer;
    OGLGraphicsPipelineInfo gpInfo;
    gpInfo.colorBlend                        = { .attachmentStates = GetOITBlending() };
    gpInfo.depthStencilState                 = { .enableDepthTest = false };
    gpInfo.shaderState.program               = shader;
    gpInfo.inputAssemblyState                = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState                = { .cullMode = GL_NONE };
    gpInfo.vertexInputState                  = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
    gpInfo.bindings.images[IMG_OIT_COLORS]   = { .texture = color, .access = GL_READ_ONLY, .format = GL_RGBA16F, .layered = true };
    gpInfo.bindings.images[IMG_OIT_VELOCITY] = { .texture = velocity, .access = GL_READ_ONLY, .format = GL_RG16F, .layered = true };
    gpInfo.bindings.images[IMG_OIT_DEPTH]    = { .texture = depth, .access = GL_READ_ONLY, .format = GL_R32UI, .layered = true };
    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;
    cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
    cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    cmdBuffer.PushCmd<OGLCmdMemoryBarrier>(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT, true);
}
