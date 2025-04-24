#include <MSG/Camera.hpp>
#include <MSG/Debug.hpp>
#include <MSG/FogArea.hpp>
#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLProgram.hpp>
#include <MSG/OGLRenderPass.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture3D.hpp>
#include <MSG/Renderer/OGL/LightCullerFwd.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/VolumetricFog.hpp>
#include <MSG/Renderer/OGL/WorleyPerlinNoise.hpp>
#include <MSG/Scene.hpp>

#include <Bindings.glsl>
#include <Fog.glsl>

#include <GL/glew.h>

#define FOG_DENSITY_WIDTH  64
#define FOG_DENSITY_HEIGHT 64
#define FOG_DENSITY_DEPTH  64

glm::uvec3 MSG::Renderer::GetDefaultVolumetricFogRes(const QualitySetting& a_Quality)
{
    static std::array<glm::uvec3, 4> s_volumetricFogResolution {
        glm::uvec3(32, 32, 16),
        glm::uvec3(64, 64, 32),
        glm::uvec3(96, 96, 64),
        glm::uvec3(128, 128, 128),
    };
    return s_volumetricFogResolution.at(int(a_Quality));
}

MSG::OGLTexture3DInfo GetParticipatingMediaTextureInfo()
{
    return {
        .width       = FOG_DENSITY_WIDTH,
        .height      = FOG_DENSITY_HEIGHT,
        .depth       = FOG_DENSITY_DEPTH,
        .sizedFormat = GL_RGBA16F
    };
}

MSG::OGLTexture3DInfo GetIntegrationTextureInfo(const glm::uvec3& a_Res)
{
    return {
        .width       = a_Res.x,
        .height      = a_Res.y,
        .depth       = a_Res.z,
        .sizedFormat = GL_RGBA16F
    };
}

std::shared_ptr<MSG::OGLTexture3D> GenerateNoiseTexture(MSG::OGLContext& a_Ctx)
{
    const uint32_t noiseRes = 64;
    MSG::Image image({
        .width     = noiseRes,
        .height    = noiseRes,
        .depth     = noiseRes,
        .pixelDesc = MSG::PixelSizedFormat::Uint8_NormalizedR,
    });
    image.Allocate();
    const float noiseFreq = 4;
    for (uint32_t z = 0; z < noiseRes; z++) {
        float noiseZ = z / float(noiseRes);
        for (uint32_t y = 0; y < noiseRes; y++) {
            float noiseY = y / float(noiseRes);
            for (uint32_t x = 0; x < noiseRes; x++) {
                float noiseX = x / float(noiseRes);
                float noise  = MSG::Renderer::WorleyPerlinNoise({ noiseX, noiseY, noiseZ }, noiseFreq);
                image.Store({ x, y, z }, glm::vec4 { noise });
            }
        }
    }
    auto texture = std::make_shared<MSG::OGLTexture3D>(a_Ctx,
        MSG::OGLTexture3DInfo {
            .width       = noiseRes,
            .height      = noiseRes,
            .depth       = noiseRes,
            .sizedFormat = GL_R8,
        });
    texture->UploadLevel(0, image);
    return texture;
}

MSG::OGLSamplerParameters GetSamplerParameters()
{
    return {
        .minFilter = GL_LINEAR,
        .magFilter = GL_LINEAR,
        .wrapS     = GL_CLAMP_TO_EDGE,
        .wrapT     = GL_CLAMP_TO_EDGE,
        .wrapR     = GL_CLAMP_TO_EDGE,
    };
}

MSG::Renderer::VolumetricFog::VolumetricFog(Renderer::Impl& a_Renderer)
    : context(a_Renderer.context)
    , fogSettingsBuffer(std::make_shared<OGLTypedBuffer<GLSL::FogSettings>>(context))
    , fogCameraBuffer(std::make_shared<OGLTypedBuffer<GLSL::CameraUBO>>(context))
    , noiseTexture(GenerateNoiseTexture(context))
    , sampler(std::make_shared<OGLSampler>(context, GetSamplerParameters()))
    , participatingMediaTexture0(std::make_shared<OGLTexture3D>(context, GetParticipatingMediaTextureInfo()))
    , participatingMediaTexture1(std::make_shared<OGLTexture3D>(context, GetParticipatingMediaTextureInfo()))
{
    renderPassInfo.name = "FogPass";
}

glm::mat4x4 GetFogCameraProj(
    const float& a_zNear,
    const float& a_zFar,
    const MSG::CameraProjectionOrthographic& a_Proj)
{
    errorFatal("Ortho projection not handled yet");
}

glm::mat4x4 GetFogCameraProj(
    const float& a_zNear,
    const float& a_zFar,
    const MSG::CameraProjectionPerspective& a_Proj)
{
    MSG::CameraProjectionPerspective fogProj = a_Proj;
    fogProj.znear                            = a_zNear;
    fogProj.zfar                             = a_zFar;
    return MSG::CameraProjection(fogProj);
}

glm::mat4x4 GetFogCameraProj(
    const float& a_zNear,
    const float& a_zFar,
    const MSG::CameraProjectionPerspectiveInfinite& a_Proj)
{
    MSG::CameraProjectionPerspective fogProj;
    fogProj.aspectRatio = a_Proj.aspectRatio;
    fogProj.fov         = a_Proj.fov;
    fogProj.znear       = a_zNear;
    fogProj.zfar        = a_zFar;
    return MSG::CameraProjection(fogProj);
}

MSG::Renderer::VolumetricFogShape ConvertFogShape(const MSG::Cube& a_Cube)
{
    return {
        .cube = {
            .center   = a_Cube.center,
            .halfSize = a_Cube.halfSize }
    };
}

MSG::Renderer::VolumetricFogShape ConvertFogShape(const MSG::Sphere& a_Sphere)
{
    return {
        .sphere = {
            .center = a_Sphere.center,
            .radius = a_Sphere.radius }
    };
}

void MSG::Renderer::VolumetricFog::Update(Renderer::Impl& a_Renderer)
{
    // if (a_Scene.GetVisibleEntities().fogAreas.empty())
    //     return;
    auto& scene                  = *a_Renderer.activeScene;
    auto& registry               = *scene.GetRegistry();
    auto& fogAreasEntities       = scene.GetVisibleEntities().fogAreas;
    auto& fogSettings            = scene.GetFogSettings();
    auto& cameraTrans            = scene.GetCamera().GetComponent<Transform>();
    auto& cameraProj             = scene.GetCamera().GetComponent<Camera>().projection;
    auto cameraUBO               = fogCameraBuffer->Get();
    cameraUBO.previous           = cameraUBO.current;
    cameraUBO.current.position   = cameraTrans.GetWorldPosition();
    cameraUBO.current.zNear      = fogSettings.volumetricFog.minDistance;
    cameraUBO.current.zFar       = fogSettings.volumetricFog.maxDistance;
    cameraUBO.current.view       = glm::inverse(cameraTrans.GetWorldTransformMatrix());
    cameraUBO.current.projection = std::visit([&fogSettings = scene.GetFogSettings().volumetricFog](auto& a_Proj) {
        return GetFogCameraProj(fogSettings.minDistance, fogSettings.maxDistance, a_Proj);
    },
        cameraProj);
    fogCameraBuffer->Set(cameraUBO);
    fogCameraBuffer->Update();
    std::swap(resultTexture, resultTexture_Previous);
    GLSL::FogSettings glslFogSettings {
        .globalScattering      = fogSettings.globalScattering,
        .globalExtinction      = fogSettings.globalExtinction,
        .globalEmissive        = fogSettings.globalEmissive,
        .globalPhaseG          = fogSettings.globalPhaseG,
        .noiseDensityOffset    = fogSettings.volumetricFog.noiseDensityOffset,
        .noiseDensityScale     = fogSettings.volumetricFog.noiseDensityScale,
        .noiseDensityIntensity = fogSettings.volumetricFog.noiseDensityIntensity,
        .noiseDensityMaxDist   = fogSettings.volumetricFog.noiseDensityMaxDist,
        .depthExponant         = fogSettings.volumetricFog.depthExp
    };
    fogSettingsBuffer->Set(glslFogSettings);
    fogSettingsBuffer->Update();

    if (fogAreasEntities.empty()) {
        fogAreaBuffer.reset();
        fogShapesBuffer.reset();
        return;
    }
    if (fogAreaBuffer == nullptr || fogAreaBuffer->size != fogAreasEntities.size())
        fogAreaBuffer = std::make_shared<OGLTypedBufferArray<GLSL::FogArea>>(context, fogAreasEntities.size());
    std::vector<VolumetricFogShape> fogShapes;
    for (size_t fogAreaIndex = 0; fogAreaIndex < fogAreasEntities.size(); fogAreaIndex++) {
        auto& fogAreaEntity             = fogAreasEntities.at(fogAreaIndex);
        auto& fogArea                   = registry.GetComponent<FogArea>(fogAreaEntity);
        auto& fogAreaTransform          = registry.GetComponent<Transform>(fogAreaEntity).GetWorldTransformMatrix();
        GLSL::FogArea glslFogArea       = fogAreaBuffer->Get(fogAreaIndex);
        glslFogArea.scattering          = fogArea.GetScattering();
        glslFogArea.extinction          = fogArea.GetExtinction();
        glslFogArea.emissive            = fogArea.GetEmissive();
        glslFogArea.phaseG              = fogArea.GetPhaseG();
        glslFogArea.attenuationExp      = fogArea.GetAttenuationExp();
        glslFogArea.op                  = uint32_t(fogArea.GetOp());
        glslFogArea.shapeComb.transform = fogAreaTransform;
        for (glslFogArea.shapeComb.count = 0;
            glslFogArea.shapeComb.count < fogArea.size() && glslFogArea.shapeComb.count < SHAPE_COMB_MAX_SHAPES;
            glslFogArea.shapeComb.count++) {
            auto& shapeCombShape     = fogArea.at(glslFogArea.shapeComb.count);
            auto& glslShapeCombShape = glslFogArea.shapeComb.shapes[glslFogArea.shapeComb.count];
            glslShapeCombShape.type  = fogArea.at(glslFogArea.shapeComb.count).index();
            glslShapeCombShape.index = fogShapes.size();
            glslShapeCombShape.op    = uint32_t(shapeCombShape.op);
            fogShapes.emplace_back(std::visit([](auto& a_Shape) { return ConvertFogShape(a_Shape); }, shapeCombShape));
        }
        fogAreaBuffer->Set(fogAreaIndex, glslFogArea);
    }
    fogAreaBuffer->Update();
    if (fogShapesBuffer == nullptr || fogShapesBuffer->size != fogShapes.size())
        fogShapesBuffer = std::make_shared<OGLTypedBufferArray<VolumetricFogShape>>(context, fogShapes.size(), fogShapes.data());
    else {
        for (size_t shapeIndex = 0; shapeIndex < fogShapes.size(); shapeIndex++)
            fogShapesBuffer->Set(shapeIndex, fogShapes.at(shapeIndex));
        fogShapesBuffer->Update();
    }
}

void MSG::Renderer::VolumetricFog::UpdateSettings(
    Renderer::Impl& a_Renderer,
    const RendererSettings& a_Settings)
{
    if (a_Settings.volumetricFogRes == resolution)
        return;
    static const glm::uvec3 lightWorkGroups(FOG_LIGHT_WORKGROUPS_X, FOG_LIGHT_WORKGROUPS_Y, FOG_LIGHT_WORKGROUPS_Z);
    static const glm::uvec3 integrationWorkGroups(FOG_INTEGRATION_WORKGROUPS_X, FOG_INTEGRATION_WORKGROUPS_Y, 1);
    checkErrorFatal(a_Settings.volumetricFogRes % lightWorkGroups != glm::uvec3(0),
        "Volumetric fog resolution is not a multiple of light injection local workgroup count");
    checkErrorFatal(a_Settings.volumetricFogRes % integrationWorkGroups != glm::uvec3(0),
        "Volumetric fog resolution is not a multiple of integration local workgroup count");
    resolution                 = a_Settings.volumetricFogRes;
    participatingMediaProgram  = a_Renderer.shaderCompiler.CompileProgram("FogParticipatingMedia");
    lightInjectionProgram      = a_Renderer.shaderCompiler.CompileProgram("FogLightsInjection");
    integrationProgram         = a_Renderer.shaderCompiler.CompileProgram("FogIntegration");
    participatingMediaTexture0 = std::make_shared<OGLTexture3D>(context, GetParticipatingMediaTextureInfo());
    participatingMediaTexture1 = std::make_shared<OGLTexture3D>(context, GetParticipatingMediaTextureInfo());
    scatteringTexture          = std::make_shared<OGLTexture3D>(context, GetIntegrationTextureInfo(resolution));
    resultTexture              = std::make_shared<OGLTexture3D>(context, GetIntegrationTextureInfo(resolution));
    resultTexture_Previous     = std::make_shared<OGLTexture3D>(context, GetIntegrationTextureInfo(resolution));
    glm::vec4 clearColor(0, 0, 0, 1);
    resultTexture->Clear(GL_RGBA, GL_FLOAT, &clearColor);
    resultTexture_Previous->Clear(GL_RGBA, GL_FLOAT, &clearColor);
}

MSG::OGLRenderPass* MSG::Renderer::VolumetricFog::GetComputePass(
    const LightCullerFwd& a_LightCuller,
    const std::shared_ptr<OGLSampler>& a_ShadowSampler,
    const std::shared_ptr<OGLBuffer>& a_FrameInfoBuffer)
{
    renderPassInfo.pipelines.clear();
    // Participating media generation pass
    {
        auto& pipeline           = renderPassInfo.pipelines.emplace_back(OGLComputePipelineInfo {});
        auto& cp                 = std::get<OGLComputePipelineInfo>(pipeline);
        cp.bindings.images.at(0) = {
            .texture = participatingMediaTexture0,
            .access  = GL_WRITE_ONLY,
            .format  = GL_RGBA16F,
            .layered = true,
        };
        cp.bindings.images.at(1) = {
            .texture = participatingMediaTexture1,
            .access  = GL_WRITE_ONLY,
            .format  = GL_RGBA16F,
            .layered = true,
        };
        cp.bindings.uniformBuffers.at(UBO_FOG_CAMERA) = {
            .buffer = fogCameraBuffer,
            .offset = 0,
            .size   = fogCameraBuffer->size
        };
        cp.bindings.uniformBuffers.at(UBO_FOG_SETTINGS) = {
            .buffer = fogSettingsBuffer,
            .offset = 0,
            .size   = fogSettingsBuffer->size
        };
        if (fogAreaBuffer != nullptr)
            cp.bindings.storageBuffers.at(0) = {
                .buffer = fogAreaBuffer,
                .offset = 0,
                .size   = fogAreaBuffer->size
            };
        if (fogShapesBuffer != nullptr)
            cp.bindings.storageBuffers.at(1) = {
                .buffer = fogShapesBuffer,
                .offset = 0,
                .size   = fogShapesBuffer->size
            };
        cp.shaderState.program = participatingMediaProgram;
        cp.workgroupX          = FOG_DENSITY_WIDTH / 8;
        cp.workgroupY          = FOG_DENSITY_HEIGHT / 8;
        cp.workgroupZ          = FOG_DENSITY_DEPTH / 8;
    }
    // Lights injection pass
    {
        auto& pipeline           = renderPassInfo.pipelines.emplace_back(OGLComputePipelineInfo {});
        auto& cp                 = std::get<OGLComputePipelineInfo>(pipeline);
        cp.bindings.images.at(0) = {
            .texture = scatteringTexture,
            .access  = GL_WRITE_ONLY,
            .format  = GL_RGBA16F,
            .layered = true,
        };
        cp.bindings.textures.at(0) = {
            .texture = participatingMediaTexture0,
            .sampler = sampler
        };
        cp.bindings.textures.at(1) = {
            .texture = participatingMediaTexture1,
            .sampler = sampler
        };
        for (auto i = 0u; i < a_LightCuller.shadows.buffer->Get().count; i++) {
            cp.bindings.textures.at(SAMPLERS_SHADOW + i) = OGLTextureBindingInfo {
                .texture = a_LightCuller.shadows.textures[i],
                .sampler = a_ShadowSampler,
            };
        }
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
        cp.bindings.uniformBuffers.at(UBO_FWD_IBL) = {
            .buffer = a_LightCuller.ibls.buffer,
            .offset = 0,
            .size   = a_LightCuller.ibls.buffer->size
        };
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
        cp.bindings.uniformBuffers.at(UBO_FOG_CAMERA) = {
            .buffer = fogCameraBuffer,
            .offset = 0,
            .size   = fogCameraBuffer->size
        };
        cp.bindings.uniformBuffers.at(UBO_FOG_SETTINGS) = {
            .buffer = fogSettingsBuffer,
            .offset = 0,
            .size   = fogSettingsBuffer->size
        };
        cp.shaderState.program = lightInjectionProgram;
        cp.workgroupX          = resolution.x / FOG_LIGHT_WORKGROUPS_X;
        cp.workgroupY          = resolution.y / FOG_LIGHT_WORKGROUPS_Y;
        cp.workgroupZ          = resolution.z / FOG_LIGHT_WORKGROUPS_Z;
    }
    // Integration pass
    {
        auto& pipeline           = renderPassInfo.pipelines.emplace_back(OGLComputePipelineInfo {});
        auto& cp                 = std::get<OGLComputePipelineInfo>(pipeline);
        cp.bindings.images.at(0) = {
            .texture = resultTexture,
            .access  = GL_WRITE_ONLY,
            .format  = GL_RGBA16F,
            .layered = true,
        };
        cp.bindings.textures.at(0) = {
            .texture = scatteringTexture,
            .sampler = sampler
        };
        cp.bindings.textures.at(1) = {
            .texture = noiseTexture
        };
        cp.bindings.textures.at(2) = {
            .texture = resultTexture_Previous,
            .sampler = sampler
        };
        cp.bindings.uniformBuffers.at(UBO_FRAME_INFO) = {
            .buffer = a_FrameInfoBuffer,
            .offset = 0,
            .size   = a_FrameInfoBuffer->size
        };
        cp.bindings.uniformBuffers.at(UBO_FOG_CAMERA) = {
            .buffer = fogCameraBuffer,
            .offset = 0,
            .size   = fogCameraBuffer->size
        };
        cp.bindings.uniformBuffers.at(UBO_FOG_SETTINGS) = {
            .buffer = fogSettingsBuffer,
            .offset = 0,
            .size   = fogSettingsBuffer->size
        };
        cp.shaderState.program = integrationProgram;
        cp.memoryBarrier       = GL_TEXTURE_UPDATE_BARRIER_BIT;
        cp.workgroupX          = resolution.x / FOG_INTEGRATION_WORKGROUPS_X;
        cp.workgroupY          = resolution.y / FOG_INTEGRATION_WORKGROUPS_Y;
        cp.workgroupZ          = 1;
    }
    return new OGLRenderPass(renderPassInfo);
}