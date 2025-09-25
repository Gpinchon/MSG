#include <MSG/Renderer/OGL/RenderPasses/DfdSubPassOITCompositing.hpp>

#include <MSG/OGLTexture3D.hpp>
#include <MSG/Renderer/OGL/RenderPassInterface.hpp>
#include <MSG/Renderer/OGL/RenderPasses/DfdSubPassOITForward.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>

#include <Bindings.glsl>
#include <OIT.glsl>

constexpr MSG::OGLColorBlendAttachmentState GetOITBlending()
{
    return {
        .index               = OUTPUT_FRAG_FWD_COMP_COLOR,
        .enableBlend         = true,
        .srcColorBlendFactor = GL_ONE,
        .dstColorBlendFactor = GL_ONE_MINUS_SRC_ALPHA,
        .srcAlphaBlendFactor = GL_SRC_ALPHA,
        .dstAlphaBlendFactor = GL_ONE
    };
}

MSG::Renderer::DfdSubPassOITCompositing::DfdSubPassOITCompositing(Renderer::Impl& a_Renderer)
    : RenderSubPassInterface({ typeid(DfdSubPassOITForward) })
    , shader(a_Renderer.shaderCompiler.CompileProgram("OITCompositing"))
{
}

void MSG::Renderer::DfdSubPassOITCompositing::Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass)
{
    auto& oitForward = a_ParentPass->Get<DfdSubPassOITForward>();
    color            = oitForward.color;
    depth            = oitForward.depth;
}

void MSG::Renderer::DfdSubPassOITCompositing::Render(Impl& a_Renderer)
{
    auto& cmdBuffer = a_Renderer.renderCmdBuffer;
    OGLGraphicsPipelineInfo gpInfo;
    gpInfo.colorBlend                      = { .attachmentStates = { GetOITBlending() } };
    gpInfo.depthStencilState               = { .enableDepthTest = false };
    gpInfo.shaderState.program             = shader;
    gpInfo.inputAssemblyState              = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState              = { .cullMode = GL_NONE };
    gpInfo.vertexInputState                = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
    gpInfo.bindings.images[IMG_OIT_COLORS] = { .texture = color, .access = GL_READ_ONLY, .format = GL_RGBA16F, .layered = true };
    gpInfo.bindings.images[IMG_OIT_DEPTH]  = { .texture = depth, .access = GL_READ_ONLY, .format = GL_R32UI, .layered = true };
    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;
    cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
    cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
}