#include <MSG/Renderer/OGL/Subsystems/FogSubsystem.hpp>

#include <MSG/Renderer/OGL/Subsystems/FrameSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsSubsystem.hpp>
#include <MSG/Renderer/SubsystemInterface.hpp>

#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/WorleyPerlinNoise.hpp>

#include <MSG/Camera.hpp>
#include <MSG/Debug.hpp>
#include <MSG/FogArea.hpp>
#include <MSG/Image.hpp>
#include <MSG/Scene.hpp>

#include <MSG/OGLCmd.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLPipeline.hpp>
#include <MSG/OGLProgram.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture3D.hpp>
#include <MSG/OGLTypedBuffer.hpp>

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

glm::uvec3 Msg::Renderer::GetDefaultVolumetricFogRes(const QualitySetting& a_Quality)
{
    return s_defaultVolumetricFogResolution.at(int(a_Quality));
}

static inline Msg::OGLTexture3DInfo GetParticipatingMediaTextureInfo()
{
    return {
        .width       = FOG_DENSITY_WIDTH,
        .height      = FOG_DENSITY_HEIGHT,
        .depth       = FOG_DENSITY_DEPTH,
        .sizedFormat = GL_RGBA16F
    };
}

static inline Msg::OGLTexture3DInfo GetIntegrationTextureInfo(const glm::uvec3& a_Res)
{
    return {
        .width       = a_Res.x,
        .height      = a_Res.y,
        .depth       = a_Res.z,
        .sizedFormat = GL_RGBA16F
    };
}

static inline std::shared_ptr<Msg::OGLTexture3D> GenerateNoiseTexture(Msg::OGLContext& a_Ctx)
{
    const uint32_t noiseRes = 64;
    Msg::Image image({
        .width     = noiseRes,
        .height    = noiseRes,
        .depth     = noiseRes,
        .pixelDesc = Msg::PixelSizedFormat::Uint8_NormalizedR,
    });
    image.Allocate();
    image.Map();
    const float noiseFreq = 4;
    for (uint32_t z = 0; z < noiseRes; z++) {
        float noiseZ = z / float(noiseRes);
        for (uint32_t y = 0; y < noiseRes; y++) {
            float noiseY = y / float(noiseRes);
            for (uint32_t x = 0; x < noiseRes; x++) {
                float noiseX = x / float(noiseRes);
                float noise  = Msg::Renderer::WorleyPerlinNoise({ noiseX, noiseY, noiseZ }, noiseFreq);
                image.Store({ x, y, z }, glm::vec4 { noise });
            }
        }
    }
    image.Unmap();
    auto texture = std::make_shared<Msg::OGLTexture3D>(a_Ctx,
        Msg::OGLTexture3DInfo {
            .width       = noiseRes,
            .height      = noiseRes,
            .depth       = noiseRes,
            .sizedFormat = GL_R8,
        });
    texture->UploadLevel(0, image);
    return texture;
}

static inline Msg::OGLSamplerParameters GetSamplerParameters()
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
    const Msg::CameraProjectionOrthographic& a_Proj)
{
    MSGErrorFatal("Ortho projection not handled yet");
}

static inline glm::mat4x4 GetFogCameraProj(
    const float& a_zNear,
    const float& a_zFar,
    const Msg::CameraProjectionPerspective& a_Proj)
{
    Msg::CameraProjectionPerspective fogProj = a_Proj;
    fogProj.znear                            = a_zNear;
    fogProj.zfar                             = a_zFar;
    return Msg::CameraProjection(fogProj);
}

static inline glm::mat4x4 GetFogCameraProj(
    const float& a_zNear,
    const float& a_zFar,
    const Msg::CameraProjectionPerspectiveInfinite& a_Proj)
{
    Msg::CameraProjectionPerspective fogProj;
    fogProj.aspectRatio = a_Proj.aspectRatio;
    fogProj.fov         = a_Proj.fov;
    fogProj.znear       = a_zNear;
    fogProj.zfar        = a_zFar;
    return Msg::CameraProjection(fogProj);
}

static inline Msg::Renderer::VolumetricFogShape ConvertFogShape(const Msg::Cube& a_Cube)
{
    return {
        .cube = {
            .center   = a_Cube.center,
            .halfSize = a_Cube.halfSize }
    };
}

static inline Msg::Renderer::VolumetricFogShape ConvertFogShape(const Msg::Sphere& a_Sphere)
{
    return {
        .sphere = {
            .center = a_Sphere.center,
            .radius = a_Sphere.radius }
    };
}

Msg::Renderer::FogSubsystem::FogSubsystem(Renderer::Impl& a_Renderer)
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

void Msg::Renderer::FogSubsystem::Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
}

void Msg::Renderer::FogSubsystem::Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
}

void Msg::Renderer::FogSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems)
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
        .globalPhaseG          = std::clamp(fogSettings.globalPhaseG, 0.f, 1.f - FLT_EPSILON),
        .noiseDensityOffset    = fogSettings.volumetricFog.noiseDensityOffset,
        .noiseDensityScale     = fogSettings.volumetricFog.noiseDensityScale,
        .noiseDensityIntensity = std::clamp(fogSettings.volumetricFog.noiseDensityIntensity, 0.f, 1.f),
        .noiseDensityMaxDist   = std::max(0.f, fogSettings.volumetricFog.noiseDensityMaxDist),
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
    if (fogShapes.empty())
        fogShapesBuffer.reset();
    else if (fogShapesBuffer == nullptr || fogShapesBuffer->size != fogShapes.size())
        fogShapesBuffer = std::make_shared<OGLTypedBufferArray<VolumetricFogShape>>(a_Renderer.context, fogShapes.size(), fogShapes.data());
    else {
        for (size_t shapeIndex = 0; shapeIndex < fogShapes.size(); shapeIndex++)
            fogShapesBuffer->Set(shapeIndex, fogShapes.at(shapeIndex));
        fogShapesBuffer->Update();
    }
    _UpdateComputePass(a_Renderer, a_Subsystems);
}

template <typename T>
static inline T Round(const T& numToRound, const T& multiple)
{
    return static_cast<T>(std::round(static_cast<double>(numToRound) / static_cast<double>(multiple)) * static_cast<double>(multiple));
}

void Msg::Renderer::FogSubsystem::UpdateSettings(
    Renderer::Impl& a_Renderer,
    const RendererSettings& a_Settings)
{
    if (a_Settings.volumetricFogRes == resolution)
        return;
    static const glm::uvec3 lightWorkGroups(FOG_LIGHT_WORKGROUPS_X, FOG_LIGHT_WORKGROUPS_Y, FOG_LIGHT_WORKGROUPS_Z);
    static const glm::uvec3 integrationWorkGroups(FOG_INTEGRATION_WORKGROUPS_X, FOG_INTEGRATION_WORKGROUPS_Y, 1);
    // round up the resolution so it is a multiple of both light and integration workgroups
    resolution = a_Settings.volumetricFogRes;
    resolution = glm::uvec3(
        Round(resolution.x, lightWorkGroups.x),
        Round(resolution.y, lightWorkGroups.y),
        Round(resolution.z, lightWorkGroups.z));
    resolution = glm::uvec3(
        Round(resolution.x, integrationWorkGroups.x),
        Round(resolution.y, integrationWorkGroups.y),
        Round(resolution.z, integrationWorkGroups.z));
    glm::ivec3 minRes = GetDefaultVolumetricFogRes(Renderer::QualitySetting::Low);
    for (int32_t cascadeIndex = 0; cascadeIndex < FOG_CASCADE_COUNT; cascadeIndex++) {
        auto& texture                      = textures[cascadeIndex];
        texture.resolution                 = glm::max(glm::ivec3(resolution) - 32 * cascadeIndex, minRes); // clamp to avoid uint overflow
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

void Msg::Renderer::FogSubsystem::_UpdateComputePass(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems)
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

void Msg::Renderer::FogSubsystem::_GetCascadePipelines(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems, const uint32_t& a_CascadeIndex)
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
        for (auto i = 0u; i < lightsSubsystem.shadows.dataBuffer->Get().count; i++) {
            auto& glslLight     = lightsSubsystem.shadows.dataBuffer->Get().shadows[i];
            auto& glslLightType = glslLight.light.commonData.type;
            auto& sampler       = glslLightType == LIGHT_TYPE_POINT ? lightsSubsystem.shadowSamplerCube : lightsSubsystem.shadowSampler;

            cp.bindings.textures.at(SAMPLERS_SHADOW + i) = OGLTextureBindingInfo {
                .texture = lightsSubsystem.shadows.texturesDepth[i],
                .sampler = sampler,
            };
        }
        cp.bindings.storageBuffers.at(SSBO_VTFS_CLUSTERS) = {
            .buffer = lightsSubsystem.vtfs.buffer->cluster,
            .offset = 0,
            .size   = lightsSubsystem.vtfs.buffer->cluster->size
        };
        cp.bindings.storageBuffers.at(SSBO_VTFS_LIGHTS) = {
            .buffer = lightsSubsystem.vtfs.buffer->lightsBuffer,
            .offset = offsetof(GLSL::VTFSLightsBuffer, lights),
            .size   = lightsSubsystem.vtfs.buffer->lightsBuffer->size
        };
        cp.bindings.storageBuffers.at(SSBO_SHADOW_DATA) = {
            .buffer = lightsSubsystem.shadows.dataBuffer,
            .offset = 0,
            .size   = lightsSubsystem.shadows.dataBuffer->size
        };
        cp.bindings.storageBuffers.at(SSBO_SHADOW_VIEWPORTS) = {
            .buffer = lightsSubsystem.shadows.viewportsBuffer,
            .offset = 0,
            .size   = lightsSubsystem.shadows.viewportsBuffer->size
        };
        cp.bindings.uniformBuffers.at(UBO_FWD_IBL) = {
            .buffer = lightsSubsystem.ibls.buffer,
            .offset = 0,
            .size   = lightsSubsystem.ibls.buffer->size
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