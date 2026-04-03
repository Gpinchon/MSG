#include <MSG/Renderer/OGL/RenderPasses/SubPassShadow.hpp>

#include <MSG/Renderer/OGL/Components/LightShadowData.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/RenderPasses/SubPassVTFS.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsShadowSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLPipelineInfo.hpp>
#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/OGLVertexArray.hpp>

#include <MSG/Light/PunctualLight.hpp>

#include <Bindings.glsl>
#include <FrameInfo.glsl>
#include <Lights.glsl>

Msg::Renderer::SubPassShadow::SubPassShadow(Renderer::Impl& a_Rdr)
    : RenderSubPassInterface({ typeid(SubPassVTFS) })
    , _cmdBuffer(a_Rdr.context, OGLCmdBufferType::OneShot)
{
}

void Msg::Renderer::SubPassShadow::Update(Renderer::Impl& a_Rdr, RenderPassInterface* a_ParentPass)
{
    auto& subsystems      = a_Rdr.subsystemsLibrary;
    auto& shadowSubsystem = subsystems.Get<LightsShadowSubsystem>();
    _render               = shadowSubsystem.countCasters > 0;
    if (!_render)
        return;
    geometryFB = a_Rdr.renderPassesLibrary.Get<PassOpaqueGeometry>().output;
    /// record the render command buffer
    auto& meshSubsystem = a_Rdr.subsystemsLibrary.Get<MeshSubsystem>();
    OGLCmdDrawInfo drawCmd;
    drawCmd.vertexCount = 3;
    OGLGraphicsPipelineInfo gpInfo;
    gpInfo.bindlessTextureSamplers.insert_range(gpInfo.bindlessTextureSamplers.end(), shadowSubsystem.textureSamplers);
    gpInfo.inputAssemblyState = { .primitiveTopology = GL_TRIANGLES };
    gpInfo.rasterizationState = { .cullMode = GL_NONE };
    gpInfo.vertexInputState   = { .vertexCount = 3, .vertexArray = a_Rdr.presentVAO };
    gpInfo.bindings           = meshSubsystem.globalBindings;
    gpInfo.bindings.images[0] = { geometryFB->info.colorBuffers[OUTPUT_FRAG_GBUFFER0].texture, GL_READ_WRITE, GL_RGBA32UI };
    gpInfo.bindings.images[1] = { geometryFB->info.colorBuffers[OUTPUT_FRAG_GBUFFER1].texture, GL_READ_WRITE, GL_RGBA32UI };
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
    _cmdBuffer.Reset();
    _cmdBuffer.Begin();
    _cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
    _cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    _cmdBuffer.End();
}

void Msg::Renderer::SubPassShadow::UpdateSettings(Renderer::Impl& a_Rdr, const RendererSettings& a_Settings)
{
    const ShaderLibrary::ProgramKeywords keywords = { { "SHADOW_QUALITY", std::to_string(int(a_Settings.shadowQuality) + 1) } };
    shader                                        = *a_Rdr.shaderCache["DeferredShadows"][keywords[0].second];
    if (shader == nullptr)
        shader = a_Rdr.shaderCompiler.CompileProgram("DeferredShadows", keywords);
}

void Msg::Renderer::SubPassShadow::Render(Impl& a_Rdr)
{
    if (_render)
        a_Rdr.renderCmdBuffer.PushCmd<OGLCmdPushCmdBuffer>(_cmdBuffer);
}
