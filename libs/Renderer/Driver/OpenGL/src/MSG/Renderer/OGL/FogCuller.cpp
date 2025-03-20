#include <MSG/FogArea.hpp>
#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLProgram.hpp>
#include <MSG/OGLRenderPass.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture3D.hpp>
#include <MSG/Renderer/OGL/FogCuller.hpp>
#include <MSG/Renderer/OGL/LightCullerFwd.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Scene.hpp>

#include <GL/glew.h>

#include <Bindings.glsl>
#include <FogCulling.glsl>

MSG::OGLTexture3DInfo GetDensityTextureInfo()
{
    return {
        .width       = FOG_DENSITY_WIDTH,
        .height      = FOG_DENSITY_HEIGHT,
        .depth       = FOG_DENSITY_DEPTH,
        .sizedFormat = GL_RGBA8
    };
}

MSG::OGLTexture3DInfo GetResultTextureInfo()
{
    return {
        .width       = FOG_WIDTH,
        .height      = FOG_HEIGHT,
        .depth       = FOG_DEPTH,
        .sizedFormat = GL_RGBA8
    };
}

MSG::ImageInfo GetImageInfo()
{
    return {
        .width     = FOG_DENSITY_WIDTH,
        .height    = FOG_DENSITY_HEIGHT,
        .depth     = FOG_DENSITY_DEPTH,
        .pixelDesc = MSG::PixelSizedFormat::Uint8_NormalizedRGBA
    };
}

MSG::Renderer::FogCuller::FogCuller(Renderer::Impl& a_Renderer)
    : context(a_Renderer.context)
    , image(GetImageInfo())
    , densityTexture(std::make_shared<OGLTexture3D>(context, GetDensityTextureInfo()))
    , resultTexture(std::make_shared<OGLTexture3D>(context, GetResultTextureInfo()))
    , cullingProgram(a_Renderer.shaderCompiler.CompileProgram("FogCulling"))
{
    image.Allocate();
    renderPassInfo.name = "FogPass";
}

MSG::OGLRenderPass* MSG::Renderer::FogCuller::Update(
    const Scene& a_Scene,
    const LightCullerFwd& a_LightCuller,
    const std::shared_ptr<OGLSampler>& a_ShadowSampler,
    const std::shared_ptr<OGLBuffer>& a_CameraBuffer,
    const std::shared_ptr<OGLBuffer>& a_FrameInfoBuffer)
{
    auto& registry        = *a_Scene.GetRegistry();
    glm::vec4 globalColor = { 1, 1, 1, 0 }; // figure out global color
    for (auto& entity : a_Scene.GetVisibleEntities().fogAreas) {
        auto& fogArea = registry.GetComponent<FogArea>(entity);
        if (glm::any(glm::isinf(fogArea.GetHalfSize()))) {
            assert(fogArea.GetGrid().GetSize() == glm::uvec3(1) && "Infinite fog sources can only have one color.");
            auto color = fogArea.GetGrid().Load({ 0, 0, 0 });
            globalColor *= glm::vec4(glm::vec3(color), 1);
            globalColor.a += color.a;
        }
    }
    if (globalColor.a == 0)
        return nullptr;
    image.Fill(globalColor); // clear image
    densityTexture->UploadLevel(0, image);
    renderPassInfo.pipelines.clear();
    auto& pipeline           = renderPassInfo.pipelines.emplace_back(OGLComputePipelineInfo {});
    auto& cp                 = std::get<OGLComputePipelineInfo>(pipeline);
    cp.bindings.images.at(0) = {
        .texture = resultTexture,
        .access  = GL_WRITE_ONLY,
        .format  = GL_RGBA8,
    };
    cp.bindings.textures.at(0) = {
        .texture = densityTexture
    };
    for (auto i = 0u; i < a_LightCuller.shadows.buffer->Get().count; i++) {
        cp.bindings.textures.at(SAMPLERS_FWD_SHADOW + i) = OGLTextureBindingInfo {
            .texture = a_LightCuller.shadows.textures[i],
            .sampler = a_ShadowSampler,
        };
    }
    cp.bindings.uniformBuffers.at(UBO_FWD_SHADOWS) = {
        .buffer = a_LightCuller.shadows.buffer,
        .offset = 0,
        .size   = a_LightCuller.shadows.buffer->size
    };
    cp.bindings.uniformBuffers.at(UBO_FRAME_INFO) = {
        .buffer = a_FrameInfoBuffer,
        .offset = 0,
        .size   = a_FrameInfoBuffer->size
    };
    cp.bindings.uniformBuffers.at(UBO_CAMERA) = {
        .buffer = a_CameraBuffer,
        .offset = 0,
        .size   = a_CameraBuffer->size
    };
    cp.bindings.storageBuffers.at(SSBO_VTFS_CLUSTERS) = {
        .buffer = a_LightCuller.vtfs.buffer.cluster,
        .offset = 0,
        .size   = a_LightCuller.vtfs.buffer.cluster->size
    };
    cp.bindings.storageBuffers.at(SSBO_VTFS_LIGHTS) = {
        .buffer = a_LightCuller.vtfs.buffer.lightsBuffer,
        .offset = offsetof(GLSL::VTFSLightsBuffer, lights),
        .size   = a_LightCuller.vtfs.buffer.lightsBuffer->size
    };
    cp.shaderState.program = cullingProgram;
    cp.memoryBarrier       = GL_TEXTURE_UPDATE_BARRIER_BIT;
    cp.workgroupX          = FOG_WIDTH;
    cp.workgroupY          = FOG_HEIGHT;
    return new OGLRenderPass(renderPassInfo);
}