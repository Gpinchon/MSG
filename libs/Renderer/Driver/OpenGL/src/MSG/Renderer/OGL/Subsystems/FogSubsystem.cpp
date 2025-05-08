#include <MSG/Renderer/OGL/Subsystems/FogSubsystem.hpp>

#include <MSG/Renderer/OGL/Subsystems/FrameSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/SubsystemLibrary.hpp>

#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/WorleyPerlinNoise.hpp>

#include <MSG/Camera.hpp>
#include <MSG/Debug.hpp>
#include <MSG/FogArea.hpp>
#include <MSG/Image.hpp>
#include <MSG/Scene.hpp>

#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLCmd.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLPipeline.hpp>
#include <MSG/OGLProgram.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture3D.hpp>

#include <Bindings.glsl>
#include <Camera.glsl>
#include <Fog.glsl>
#include <FogArea.glsl>
#include <FogCamera.glsl>
#include <FrameInfo.glsl>
#include <LightsIBLInputs.glsl>
#include <LightsShadowInputs.glsl>

#include <GL/glew.h>

#define FOG_DENSITY_WIDTH  64
#define FOG_DENSITY_HEIGHT 64
#define FOG_DENSITY_DEPTH  64

static std::array<glm::uvec3, 4> s_defaultVolumetricFogResolution {
    glm::uvec3(32, 32, 16),
    glm::uvec3(64, 64, 32),
    glm::uvec3(96, 96, 64),
    glm::uvec3(128, 128, 128),
};

glm::uvec3 MSG::Renderer::GetDefaultVolumetricFogRes(const QualitySetting& a_Quality)
{
    return s_defaultVolumetricFogResolution.at(int(a_Quality));
}

static inline MSG::OGLTexture3DInfo GetParticipatingMediaTextureInfo()
{
    return {
        .width       = FOG_DENSITY_WIDTH,
        .height      = FOG_DENSITY_HEIGHT,
        .depth       = FOG_DENSITY_DEPTH,
        .sizedFormat = GL_RGBA16F
    };
}

static inline MSG::OGLTexture3DInfo GetIntegrationTextureInfo(const glm::uvec3& a_Res)
{
    return {
        .width       = a_Res.x,
        .height      = a_Res.y,
        .depth       = a_Res.z,
        .sizedFormat = GL_RGBA16F
    };
}

static inline std::shared_ptr<MSG::OGLTexture3D> GenerateNoiseTexture(MSG::OGLContext& a_Ctx)
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

static inline MSG::OGLSamplerParameters GetSamplerParameters()
{
    return {
        .minFilter = GL_LINEAR,
        .magFilter = GL_LINEAR,
        .wrapS     = GL_CLAMP_TO_EDGE,
        .wrapT     = GL_CLAMP_TO_EDGE,
        .wrapR     = GL_CLAMP_TO_EDGE,
    };
}

static inline glm::mat4x4 GetFogCameraProj(
    const float& a_zNear,
    const float& a_zFar,
    const MSG::CameraProjectionOrthographic& a_Proj)
{
    errorFatal("Ortho projection not handled yet");
}

static inline glm::mat4x4 GetFogCameraProj(
    const float& a_zNear,
    const float& a_zFar,
    const MSG::CameraProjectionPerspective& a_Proj)
{
    MSG::CameraProjectionPerspective fogProj = a_Proj;
    fogProj.znear                            = a_zNear;
    fogProj.zfar                             = a_zFar;
    return MSG::CameraProjection(fogProj);
}

static inline glm::mat4x4 GetFogCameraProj(
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

static inline MSG::Renderer::VolumetricFogShape ConvertFogShape(const MSG::Cube& a_Cube)
{
    return {
        .cube = {
            .center   = a_Cube.center,
            .halfSize = a_Cube.halfSize }
    };
}

static inline MSG::Renderer::VolumetricFogShape ConvertFogShape(const MSG::Sphere& a_Sphere)
{
    return {
        .sphere = {
            .center = a_Sphere.center,
            .radius = a_Sphere.radius }
    };
}

MSG::Renderer::FogSubsystem::FogSubsystem(Renderer::Impl& a_Renderer)
    : SubsystemInterface({ typeid(FrameSubsystem), typeid(LightsSubsystem) })
    , cascadeZero(std::make_shared<OGLTexture3D>(a_Renderer.context, OGLTexture3DInfo { .sizedFormat = GL_RGBA16F }))
    , fogSettingsBuffer(std::make_shared<OGLTypedBuffer<GLSL::FogSettings>>(a_Renderer.context))
    , fogCamerasBuffer(std::make_shared<OGLTypedBufferArray<GLSL::FogCamera>>(a_Renderer.context, FOG_CASCADE_COUNT))
    , noiseTexture(GenerateNoiseTexture(a_Renderer.context))
    , sampler(std::make_shared<OGLSampler>(a_Renderer.context, GetSamplerParameters()))
    , _cmdBuffer(a_Renderer.context)
    , _programParticipating(a_Renderer.shaderCompiler.CompileProgram("FogParticipatingMedia"))
    , _programLightsInject(a_Renderer.shaderCompiler.CompileProgram("FogLightsInjection"))
    , _programIntegration(a_Renderer.shaderCompiler.CompileProgram("FogIntegration"))
{
    for (auto& texture : textures) {
        texture.participatingMediaTexture0 = std::make_shared<OGLTexture3D>(a_Renderer.context, GetParticipatingMediaTextureInfo());
        texture.participatingMediaTexture1 = std::make_shared<OGLTexture3D>(a_Renderer.context, GetParticipatingMediaTextureInfo());
    }
    glm::vec4 clearColor(0, 0, 0, 1);
    cascadeZero->Clear(GL_RGBA, GL_FLOAT, &clearColor);
}

void MSG::Renderer::FogSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems)
{
    auto& scene            = *a_Renderer.activeScene;
    auto& registry         = *scene.GetRegistry();
    auto& fogAreasEntities = scene.GetVisibleEntities().fogAreas;
    auto& fogSettings      = scene.GetFogSettings();
    auto& cameraTrans      = scene.GetCamera().GetComponent<Transform>();
    auto& cameraProj       = scene.GetCamera().GetComponent<Camera>().projection;
    float frustumDepth     = fogSettings.volumetricFog.maxDistance - fogSettings.volumetricFog.minDistance;
    float cascadeDepth     = frustumDepth / float(FOG_CASCADE_COUNT);
    float cascadeStart     = fogSettings.volumetricFog.minDistance;
    for (uint32_t cascadeIndex = 0; cascadeIndex < FOG_CASCADE_COUNT; cascadeIndex++) {
        float cascadeEnd             = cascadeStart + cascadeDepth;
        auto cameraUBO               = fogCamerasBuffer->Get(cascadeIndex);
        cameraUBO.previous           = cameraUBO.current;
        cameraUBO.current.position   = cameraTrans.GetWorldPosition();
        cameraUBO.current.zNear      = cascadeStart - 0.1f * cascadeDepth * cascadeIndex;
        cameraUBO.current.zFar       = cascadeEnd;
        cameraUBO.current.view       = glm::inverse(cameraTrans.GetWorldTransformMatrix());
        cameraUBO.current.projection = std::visit([&cam = cameraUBO.current](auto& a_Proj) {
            return GetFogCameraProj(cam.zNear, cam.zFar, a_Proj);
        },
            cameraProj);
        fogCamerasBuffer->Set(cascadeIndex, cameraUBO);
        std::swap(textures[cascadeIndex].resultTexture, textures[cascadeIndex].resultTexture_Previous);
        cascadeStart = cascadeEnd;
    }
    fogCamerasBuffer->Update();

    GLSL::FogSettings glslFogSettings {
        .globalScattering      = fogSettings.globalScattering,
        .globalExtinction      = fogSettings.globalExtinction,
        .globalEmissive        = fogSettings.globalEmissive,
        .globalPhaseG          = fogSettings.globalPhaseG,
        .noiseDensityOffset    = fogSettings.volumetricFog.noiseDensityOffset,
        .noiseDensityScale     = fogSettings.volumetricFog.noiseDensityScale,
        .noiseDensityIntensity = fogSettings.volumetricFog.noiseDensityIntensity,
        .noiseDensityMaxDist   = fogSettings.volumetricFog.noiseDensityMaxDist,
        .depthExponant         = fogSettings.volumetricFog.depthExp,
        .fogBackground         = fogSettings.fogBackground
    };
    fogSettingsBuffer->Set(glslFogSettings);
    fogSettingsBuffer->Update();

    if (fogAreasEntities.empty()) {
        fogAreaBuffer.reset();
        fogShapesBuffer.reset();
        _UpdateComputePass(a_Renderer, a_Subsystems);
        return;
    } else if (fogAreaBuffer == nullptr || fogAreaBuffer->size != fogAreasEntities.size())
        fogAreaBuffer = std::make_shared<OGLTypedBufferArray<GLSL::FogArea>>(a_Renderer.context, fogAreasEntities.size());
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
        fogShapesBuffer = std::make_shared<OGLTypedBufferArray<VolumetricFogShape>>(a_Renderer.context, fogShapes.size(), fogShapes.data());
    else {
        for (size_t shapeIndex = 0; shapeIndex < fogShapes.size(); shapeIndex++)
            fogShapesBuffer->Set(shapeIndex, fogShapes.at(shapeIndex));
        fogShapesBuffer->Update();
    }
    _UpdateComputePass(a_Renderer, a_Subsystems);
}

void MSG::Renderer::FogSubsystem::UpdateSettings(
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
    resolution = a_Settings.volumetricFogRes;
    for (uint32_t cascadeIndex = 0; cascadeIndex < FOG_CASCADE_COUNT; cascadeIndex++) {
        auto& texture                      = textures[cascadeIndex];
        texture.resolution                 = glm::clamp(resolution - 32u * cascadeIndex, GetDefaultVolumetricFogRes(Renderer::QualitySetting::Low), resolution);
        texture.participatingMediaTexture0 = std::make_shared<OGLTexture3D>(a_Renderer.context, GetParticipatingMediaTextureInfo());
        texture.participatingMediaTexture1 = std::make_shared<OGLTexture3D>(a_Renderer.context, GetParticipatingMediaTextureInfo());
        texture.scatteringTexture          = std::make_shared<OGLTexture3D>(a_Renderer.context, GetIntegrationTextureInfo(texture.resolution));
        texture.resultTexture              = std::make_shared<OGLTexture3D>(a_Renderer.context, GetIntegrationTextureInfo(texture.resolution));
        texture.resultTexture_Previous     = std::make_shared<OGLTexture3D>(a_Renderer.context, GetIntegrationTextureInfo(texture.resolution));
        glm::vec4 clearColor(0, 0, 0, 1);
        texture.resultTexture->Clear(GL_RGBA, GL_FLOAT, &clearColor);
        texture.resultTexture_Previous->Clear(GL_RGBA, GL_FLOAT, &clearColor);
    }
}

void MSG::Renderer::FogSubsystem::_UpdateComputePass(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems)
{
    _executionFence.Wait();
    _executionFence.Reset();
    _cmdBuffer.Reset();
    _cmdBuffer.Begin();
    for (uint32_t cascadeIndex = 0; cascadeIndex < FOG_CASCADE_COUNT; cascadeIndex++)
        _GetCascadePipelines(a_Renderer, a_Subsystems, cascadeIndex);
    _cmdBuffer.End();
    _cmdBuffer.Execute(&_executionFence);
}

void MSG::Renderer::FogSubsystem::_GetCascadePipelines(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems, const uint32_t& a_CascadeIndex)
{
    auto& lightsSubsystem = a_Subsystems.Get<LightsSubsystem>();
    auto& frameSubsystem  = a_Subsystems.Get<FrameSubsystem>();
    // Participating media generation pass
    {
        OGLComputePipelineInfo cp;
        cp.bindings.images.at(0) = {
            .texture = textures[a_CascadeIndex].participatingMediaTexture0,
            .access  = GL_WRITE_ONLY,
            .format  = GL_RGBA16F,
            .layered = true,
        };
        cp.bindings.images.at(1) = {
            .texture = textures[a_CascadeIndex].participatingMediaTexture1,
            .access  = GL_WRITE_ONLY,
            .format  = GL_RGBA16F,
            .layered = true,
        };
        cp.bindings.uniformBuffers.at(UBO_FOG_CAMERA) = {
            .buffer = fogCamerasBuffer,
            .offset = uint32_t(fogCamerasBuffer->value_size * a_CascadeIndex),
            .size   = uint32_t(fogCamerasBuffer->value_size)
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
        cp.shaderState.program = _programParticipating;
        _cmdBuffer.PushCmd<OGLCmdPushPipeline>(cp);
        _cmdBuffer.PushCmd<OGLCmdDispatchCompute>(OGLCmdDispatchComputeInfo {
            .workgroupX = uint16_t(FOG_DENSITY_WIDTH / 8),
            .workgroupY = uint16_t(FOG_DENSITY_HEIGHT / 8),
            .workgroupZ = uint16_t(FOG_DENSITY_DEPTH / 8),
        });
    }
    // Lights injection pass
    {
        OGLComputePipelineInfo cp;
        cp.bindings.images.at(0) = {
            .texture = textures[a_CascadeIndex].scatteringTexture,
            .access  = GL_WRITE_ONLY,
            .format  = GL_RGBA16F,
            .layered = true,
        };
        cp.bindings.textures.at(0) = {
            .texture = textures[a_CascadeIndex].participatingMediaTexture0,
            .sampler = sampler
        };
        cp.bindings.textures.at(1) = {
            .texture = textures[a_CascadeIndex].participatingMediaTexture1,
            .sampler = sampler
        };
        for (auto i = 0u; i < lightsSubsystem.shadows.buffer->Get().count; i++) {
            cp.bindings.textures.at(SAMPLERS_SHADOW + i) = OGLTextureBindingInfo {
                .texture = lightsSubsystem.shadows.texturesMoments[i],
                .sampler = lightsSubsystem.shadowSampler,
            };
        }
        cp.bindings.storageBuffers.at(SSBO_VTFS_CLUSTERS) = {
            .buffer = lightsSubsystem.vtfs.buffer.cluster,
            .offset = 0,
            .size   = lightsSubsystem.vtfs.buffer.cluster->size
        };
        cp.bindings.storageBuffers.at(SSBO_VTFS_LIGHTS) = {
            .buffer = lightsSubsystem.vtfs.buffer.lightsBuffer,
            .offset = offsetof(GLSL::VTFSLightsBuffer, lights),
            .size   = lightsSubsystem.vtfs.buffer.lightsBuffer->size
        };
        cp.bindings.uniformBuffers.at(UBO_FWD_IBL) = {
            .buffer = lightsSubsystem.ibls.buffer,
            .offset = 0,
            .size   = lightsSubsystem.ibls.buffer->size
        };
        cp.bindings.uniformBuffers.at(UBO_FWD_SHADOWS) = {
            .buffer = lightsSubsystem.shadows.buffer,
            .offset = 0,
            .size   = lightsSubsystem.shadows.buffer->size
        };
        cp.bindings.uniformBuffers.at(UBO_FRAME_INFO) = {
            .buffer = frameSubsystem.buffer,
            .offset = 0,
            .size   = frameSubsystem.buffer->size
        };
        cp.bindings.uniformBuffers.at(UBO_FOG_CAMERA) = {
            .buffer = fogCamerasBuffer,
            .offset = uint32_t(fogCamerasBuffer->value_size * a_CascadeIndex),
            .size   = uint32_t(fogCamerasBuffer->value_size)
        };
        cp.bindings.uniformBuffers.at(UBO_FOG_SETTINGS) = {
            .buffer = fogSettingsBuffer,
            .offset = 0,
            .size   = fogSettingsBuffer->size
        };
        cp.shaderState.program = _programLightsInject;
        _cmdBuffer.PushCmd<OGLCmdPushPipeline>(cp);
        _cmdBuffer.PushCmd<OGLCmdDispatchCompute>(OGLCmdDispatchComputeInfo {
            .workgroupX = uint16_t(textures[a_CascadeIndex].resolution.x / FOG_LIGHT_WORKGROUPS_X),
            .workgroupY = uint16_t(textures[a_CascadeIndex].resolution.y / FOG_LIGHT_WORKGROUPS_Y),
            .workgroupZ = uint16_t(textures[a_CascadeIndex].resolution.z / FOG_LIGHT_WORKGROUPS_Z),
        });
    }
    // Integration pass
    {
        OGLComputePipelineInfo cp;
        cp.bindings.images.at(0) = {
            .texture = textures[a_CascadeIndex].resultTexture,
            .access  = GL_WRITE_ONLY,
            .format  = GL_RGBA16F,
            .layered = true,
        };
        cp.bindings.textures.at(0) = {
            .texture = textures[a_CascadeIndex].scatteringTexture,
            .sampler = sampler
        };
        cp.bindings.textures.at(1) = {
            .texture = noiseTexture
        };
        cp.bindings.textures.at(2) = {
            .texture = textures[a_CascadeIndex].resultTexture_Previous,
            .sampler = sampler
        };
        cp.bindings.uniformBuffers.at(UBO_FRAME_INFO) = {
            .buffer = frameSubsystem.buffer,
            .offset = 0,
            .size   = frameSubsystem.buffer->size
        };
        cp.bindings.uniformBuffers.at(UBO_FOG_CAMERA) = {
            .buffer = fogCamerasBuffer,
            .offset = uint32_t(fogCamerasBuffer->value_size * a_CascadeIndex),
            .size   = uint32_t(fogCamerasBuffer->value_size)
        };
        cp.bindings.uniformBuffers.at(UBO_FOG_SETTINGS) = {
            .buffer = fogSettingsBuffer,
            .offset = 0,
            .size   = fogSettingsBuffer->size
        };
        if (a_CascadeIndex > 0)
            cp.bindings.textures.at(3) = {
                .texture = textures[a_CascadeIndex - 1].resultTexture,
                .sampler = sampler
            };
        else
            cp.bindings.textures.at(3) = {
                .texture = cascadeZero,
                .sampler = sampler
            };
        cp.shaderState.program = _programIntegration;
        cp.memoryBarrier       = GL_TEXTURE_UPDATE_BARRIER_BIT;
        _cmdBuffer.PushCmd<OGLCmdPushPipeline>(cp);
        _cmdBuffer.PushCmd<OGLCmdDispatchCompute>(OGLCmdDispatchComputeInfo {
            .workgroupX = uint16_t(textures[a_CascadeIndex].resolution.x / FOG_INTEGRATION_WORKGROUPS_X),
            .workgroupY = uint16_t(textures[a_CascadeIndex].resolution.y / FOG_INTEGRATION_WORKGROUPS_Y),
            .workgroupZ = 1,
        });
    }
}