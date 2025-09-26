#include <MSG/Renderer/OGL/RenderPasses/DfdSubPassSkybox.hpp>

#include <MSG/Renderer/OGL/RenderPasses/DfdSubPassOpaqueGeometry.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>
#include <MSG/Renderer/RenderPassInterface.hpp>
#include <MSG/Scene.hpp>

#include <Bindings.glsl>

MSG::Renderer::DfdSubPassSkybox::DfdSubPassSkybox(Renderer::Impl& a_Renderer)
    : RenderSubPassInterface({ typeid(DfdSubPassOpaqueGeometry) })
    , shader(a_Renderer.shaderCompiler.CompileProgram("DeferredSkybox"))
{
}

void MSG::Renderer::DfdSubPassSkybox::Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass)
{
}

void MSG::Renderer::DfdSubPassSkybox::Render(Impl& a_Renderer)
{
    auto& activeScene   = *a_Renderer.activeScene;
    auto& cmdBuffer     = a_Renderer.renderCmdBuffer;
    auto& meshSubsystem = a_Renderer.subsystemsLibrary.Get<MeshSubsystem>();
    // RENDER SKYBOX IF NEEDED
    if (activeScene.GetSkybox().texture != nullptr) {
        auto skybox  = a_Renderer.LoadTexture(activeScene.GetSkybox().texture.get());
        auto sampler = activeScene.GetSkybox().sampler != nullptr ? a_Renderer.LoadSampler(activeScene.GetSkybox().sampler.get()) : nullptr;
        OGLGraphicsPipelineInfo gpInfo;
        gpInfo.shaderState.program                 = shader;
        gpInfo.vertexInputState                    = { .vertexCount = 3, .vertexArray = a_Renderer.presentVAO };
        gpInfo.inputAssemblyState                  = { .primitiveTopology = GL_TRIANGLES };
        gpInfo.rasterizationState                  = { .cullMode = GL_NONE };
        gpInfo.bindings                            = meshSubsystem.globalBindings;
        gpInfo.bindings.textures[SAMPLERS_SKYBOX]  = { skybox, sampler };
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