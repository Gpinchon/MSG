#include <MSG/Renderer/OGL/RenderPasses/SubPassOpaqueGeometry.hpp>

#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

#include <Material.glsl>

Msg::Renderer::SubPassOpaqueGeometry::SubPassOpaqueGeometry(Renderer::Impl& a_Rdr)
    : RenderSubPassInterface(/* NO DEPENDENCIES */)
    , _cmdBuffer(a_Rdr.context)
{
}

void Msg::Renderer::SubPassOpaqueGeometry::Update(Renderer::Impl& a_Rdr, RenderPassInterface* a_ParentPass)
{
    auto& meshSubsystem = a_Rdr.subsystemsLibrary.Get<MeshSubsystem>();
    // NOW WE RENDER OPAQUE OBJECTS
    _cmdBuffer.Reset();
    _cmdBuffer.Begin();
    for (auto& mesh : meshSubsystem.opaque) {
        auto shader = a_Rdr.shaderCompiler.CompileProgram("DeferredGeometry",
            ShaderLibrary::ProgramKeyword { TO_STRING(SKINNED), mesh.isSkinned ? "1" : "0" },
            ShaderLibrary::ProgramKeyword { TO_STRING(MATERIAL_TYPE), GLSL::MaterialTypeToString(mesh.materialType) },
            ShaderLibrary::ProgramKeyword { TO_STRING(MATERIAL_UNLIT), mesh.isUnlit ? "1" : "0" });

        OGLGraphicsPipelineInfo gpInfo             = mesh.pipeline;
        gpInfo.depthStencilState.enableStencilTest = true;
        gpInfo.depthStencilState.front.passOp      = GL_REPLACE;
        gpInfo.depthStencilState.front.reference   = 255;
        gpInfo.depthStencilState.back              = gpInfo.depthStencilState.front;
        gpInfo.shaderState.program                 = shader;
        _cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
        _cmdBuffer.PushCmd<OGLCmdDraw>(mesh.drawCmd);
    }
    _cmdBuffer.End();
}

void Msg::Renderer::SubPassOpaqueGeometry::Render(Impl& a_Rdr)
{
    a_Rdr.renderCmdBuffer.PushCmd<OGLCmdPushCmdBuffer>(_cmdBuffer);
}
