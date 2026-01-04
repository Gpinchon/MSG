#include <MSG/Renderer/OGL/RenderPasses/SubPassSkybox.hpp>

#include <MSG/OGLSampler.hpp>
#include <MSG/Renderer/OGL/RenderPasses/SubPassOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>
#include <MSG/Renderer/RenderPassInterface.hpp>
#include <MSG/Scene.hpp>

#include <Bindings.glsl>

static constexpr Msg::OGLSamplerParameters GetSamplerParameters()
{
    Msg::OGLSamplerParameters parameters;
    parameters.seamlessCubemap = true;
    return parameters;
}

Msg::Renderer::SubPassSkybox::SubPassSkybox(Renderer::Impl& a_Renderer)
    : RenderSubPassInterface({ typeid(SubPassOpaqueGeometry) })
    , shader(a_Renderer.shaderCompiler.CompileProgram("DeferredSkybox"))
    , sampler(std::make_shared<OGLSampler>(a_Renderer.context, GetSamplerParameters()))
{
}

void Msg::Renderer::SubPassSkybox::Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass)
{
    auto& activeScene = *a_Renderer.activeScene;
    if (activeScene.GetSkybox() != nullptr)
        texture = a_Renderer.LoadTexture(activeScene.GetSkybox().get());
}

void Msg::Renderer::SubPassSkybox::Render(Impl& a_Renderer)
{
    auto& activeScene   = *a_Renderer.activeScene;
    auto& cmdBuffer     = a_Renderer.renderCmdBuffer;
    auto& meshSubsystem = a_Renderer.subsystemsLibrary.Get<MeshSubsystem>();
    // RENDER SKYBOX IF NEEDED
    if (activeScene.GetSkybox() != nullptr) {
        OGLGraphicsPipelineInfo gpInfo;
        gpInfo.shaderState.program                 = shader;
        gpInfo.vertexInputState                    = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
        gpInfo.inputAssemblyState                  = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.rasterizationState                  = { .cullMode = GL_NONE };
        gpInfo.bindings                            = meshSubsystem.globalBindings;
        gpInfo.bindings.textures[SAMPLERS_SKYBOX]  = { texture, sampler };
        gpInfo.depthStencilState.enableDepthTest   = false;
        gpInfo.depthStencilState.enableStencilTest = true;
        gpInfo.depthStencilState.front.compareOp   = GL_NOTEQUAL;
        gpInfo.depthStencilState.front.reference   = 255;
        gpInfo.depthStencilState.back              = gpInfo.depthStencilState.front;
        OGLCmdDrawInfo drawCmd;
        drawCmd.vertexCount = 3;
        cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
        cmdBuffer.PushCmd<OGLCmdDraw>(drawCmd);
    }
}