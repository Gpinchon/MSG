#include <MSG/Renderer/OGL/RenderPasses/SubPassOpaqueGeometry.hpp>

#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

Msg::Renderer::SubPassOpaqueGeometry::SubPassOpaqueGeometry()
    : RenderSubPassInterface(/* NO DEPENDENCIES */)
{
}

void Msg::Renderer::SubPassOpaqueGeometry::Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass)
{
}

void Msg::Renderer::SubPassOpaqueGeometry::Render(Impl& a_Renderer)
{
    auto& meshSubsystem = a_Renderer.subsystemsLibrary.Get<MeshSubsystem>();
    auto& cmdBuffer     = a_Renderer.renderCmdBuffer;
    // NOW WE RENDER OPAQUE OBJECTS
    for (auto& mesh : meshSubsystem.opaque) {
        ShaderLibrary::ProgramKeywords keywords(3);
        keywords[0] = { "SKINNED", mesh.isSkinned ? "1" : "0" };
        if (mesh.isMetRough)
            keywords[1] = { "MATERIAL_TYPE", "MATERIAL_TYPE_METALLIC_ROUGHNESS" };
        else if (mesh.isSpecGloss)
            keywords[1] = { "MATERIAL_TYPE", "MATERIAL_TYPE_SPECULAR_GLOSSINESS" };
        keywords[2]  = { "MATERIAL_UNLIT", mesh.isUnlit ? "1" : "0" };
        auto& shader = *a_Renderer.shaderCache["DeferredGeometry"][keywords[0].second][keywords[1].second][keywords[2].second];
        if (!shader)
            shader = a_Renderer.shaderCompiler.CompileProgram("DeferredGeometry", keywords);
        OGLGraphicsPipelineInfo gpInfo             = mesh.pipeline;
        gpInfo.depthStencilState.enableStencilTest = true;
        gpInfo.depthStencilState.front.passOp      = GL_REPLACE;
        gpInfo.depthStencilState.front.reference   = 255;
        gpInfo.depthStencilState.back              = gpInfo.depthStencilState.front;
        gpInfo.shaderState.program                 = shader;
        cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
        cmdBuffer.PushCmd<OGLCmdDraw>(mesh.drawCmd);
    }
}