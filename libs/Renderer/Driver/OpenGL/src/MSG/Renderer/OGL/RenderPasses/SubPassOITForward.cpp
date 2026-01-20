#include <MSG/Renderer/OGL/RenderPasses/SubPassOITForward.hpp>

#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLTexture3D.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/RenderPasses/PassBlendedGeometry.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/MeshSubsystem.hpp>

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
            .srcAlphaBlendFactor = GL_SRC_ALPHA,
            .dstAlphaBlendFactor = GL_ONE },
    };
}

void Msg::Renderer::SubPassOITForward::Update(Renderer::Impl& a_Renderer, RenderPassInterface* a_ParentPass)
{
    auto& renderBuffer      = *a_Renderer.activeRenderBuffer;
    auto renderBufferSize   = glm::uvec2(renderBuffer->width, renderBuffer->height);
    glm::uvec2 internalSize = glm::vec2(renderBufferSize) * a_Renderer.settings.internalResolution;
    auto fbSize             = depth != nullptr ? glm::uvec2(depth->width, depth->height) : glm::uvec2(0);
    if (fbSize != internalSize) {
        depth = std::make_shared<OGLTexture3D>(
            a_Renderer.context,
            OGLTexture3DInfo {
                .width       = internalSize.x,
                .height      = internalSize.y,
                .depth       = OIT_LAYERS,
                .sizedFormat = GL_R32UI,
            });
        velocity = std::make_shared<OGLTexture3D>(
            a_Renderer.context,
            OGLTexture3DInfo {
                .width       = internalSize.x,
                .height      = internalSize.y,
                .depth       = OIT_LAYERS,
                .sizedFormat = GL_RG16F,
            });
        gBuffer0 = std::make_shared<OGLTexture3D>(
            a_Renderer.context,
            OGLTexture3DInfo {
                .width       = internalSize.x,
                .height      = internalSize.y,
                .depth       = OIT_LAYERS,
                .sizedFormat = GL_RGBA32UI,
            });
        gBuffer1 = std::make_shared<OGLTexture3D>(
            a_Renderer.context,
            OGLTexture3DInfo {
                .width       = internalSize.x,
                .height      = internalSize.y,
                .depth       = OIT_LAYERS,
                .sizedFormat = GL_RGBA32UI,
            });
    }
}

void Msg::Renderer::SubPassOITForward::Render(Impl& a_Renderer)
{
    auto& meshSubsystem = a_Renderer.subsystemsLibrary.Get<MeshSubsystem>();
    auto& cmdBuffer     = a_Renderer.renderCmdBuffer;
    auto shadowQuality  = std::to_string(int(a_Renderer.settings.shadowQuality) + 1);

    cmdBuffer.PushCmd<OGLCmdClearTexture>(depth,
        OGLClearTextureInfo {
            .size  = { depth->width, depth->height, depth->depth },
            .value = glm::uvec4(std::numeric_limits<uint32_t>::max()),
        });
    // RENDER DEPTH
    for (auto& mesh : meshSubsystem.blended) {
        ShaderLibrary::ProgramKeywords keywords(2);
        keywords[0] = { "SKINNED", mesh.isSkinned ? "1" : "0" };
        if (mesh.isMetRough)
            keywords[1] = { "MATERIAL_TYPE", "MATERIAL_TYPE_METALLIC_ROUGHNESS" };
        else if (mesh.isSpecGloss)
            keywords[1] = { "MATERIAL_TYPE", "MATERIAL_TYPE_SPECULAR_GLOSSINESS" };
        auto& shader = *a_Renderer.shaderCache["OITDepth"][keywords[0].second][keywords[1].second];
        if (!shader)
            shader = a_Renderer.shaderCompiler.CompileProgram("OITDepth", keywords);
        OGLGraphicsPipelineInfo gpInfo            = mesh.pipeline;
        gpInfo.shaderState.program                = shader;
        gpInfo.colorBlend                         = { .attachmentStates = GetOITBlending() };
        gpInfo.depthStencilState.enableDepthWrite = false;
        gpInfo.bindings.images[IMG_OIT_DEPTH]     = { .texture = depth, .access = GL_READ_WRITE, .format = GL_R32UI, .layered = true };
        cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
        cmdBuffer.PushCmd<OGLCmdDraw>(mesh.drawCmd);
    }
    cmdBuffer.PushCmd<OGLCmdMemoryBarrier>(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT, true);
    // RENDER SURFACES
    for (auto& mesh : meshSubsystem.blended) {
        ShaderLibrary::ProgramKeywords keywords(4);
        keywords[0] = { "SKINNED", mesh.isSkinned ? "1" : "0" };
        if (mesh.isMetRough)
            keywords[1] = { "MATERIAL_TYPE", "MATERIAL_TYPE_METALLIC_ROUGHNESS" };
        else if (mesh.isSpecGloss)
            keywords[1] = { "MATERIAL_TYPE", "MATERIAL_TYPE_SPECULAR_GLOSSINESS" };
        keywords[2]  = { "MATERIAL_UNLIT", mesh.isUnlit ? "1" : "0" };
        keywords[3]  = { "SHADOW_QUALITY", shadowQuality };
        auto& shader = *a_Renderer.shaderCache["OITForward"][keywords[0].second][keywords[1].second][keywords[2].second][keywords[3].second];
        if (!shader)
            shader = a_Renderer.shaderCompiler.CompileProgram("OITForward", keywords);
        OGLGraphicsPipelineInfo gpInfo            = mesh.pipeline;
        gpInfo.shaderState.program                = shader;
        gpInfo.colorBlend                         = { .attachmentStates = { GetOITBlending() } };
        gpInfo.depthStencilState.enableDepthWrite = false;
        gpInfo.bindings.images[IMG_OIT_VELOCITY]  = { .texture = velocity, .access = GL_WRITE_ONLY, .format = GL_RG16F, .layered = true };
        gpInfo.bindings.images[IMG_OIT_GBUFFER0]  = { .texture = gBuffer0, .access = GL_WRITE_ONLY, .format = GL_RGBA32UI, .layered = true };
        gpInfo.bindings.images[IMG_OIT_GBUFFER1]  = { .texture = gBuffer1, .access = GL_WRITE_ONLY, .format = GL_RGBA32UI, .layered = true };
        gpInfo.bindings.images[IMG_OIT_DEPTH]     = { .texture = depth, .access = GL_READ_ONLY, .format = GL_R32F, .layered = true };
        cmdBuffer.PushCmd<OGLCmdPushPipeline>(gpInfo);
        cmdBuffer.PushCmd<OGLCmdDraw>(mesh.drawCmd);
    }
    cmdBuffer.PushCmd<OGLCmdMemoryBarrier>(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT, true);
}