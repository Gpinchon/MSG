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
#include <MSG/Scene.hpp>

#include <Bindings.glsl>
#include <Fog.glsl>

#include <GL/glew.h>

#define FOG_DENSITY_WIDTH  32
#define FOG_DENSITY_HEIGHT 32
#define FOG_DENSITY_DEPTH  32

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

// Hash by David_Hoskins
#define UI0 1597334673U
#define UI1 3812015801U
#define UI2 glm::uvec2(UI0, UI1)
#define UI3 glm::uvec3(UI0, UI1, 2798796415U)
#define UIF (1.f / float(0xffffffffU))
glm::vec3 hash33(glm::vec3 p)
{
    glm::uvec3 q = glm::uvec3(glm::ivec3(p)) * UI3;
    q            = (q.x ^ q.y ^ q.z) * UI3;
    return -1.f + 2.f * glm::vec3(q) * UIF;
}

float remap(float x, float a, float b, float c, float d)
{
    return (((x - a) / (b - a)) * (d - c)) + c;
}

// Gradient noise by iq (modified to be tileable)
float gradientNoise(glm::vec3 x, float freq)
{
    // grid
    glm::vec3 p = floor(x);
    glm::vec3 w = fract(x);

    // quintic interpolant
    glm::vec3 u = w * w * w * (w * (w * 6.f - 15.f) + 10.f);

    // gradients
    glm::vec3 ga = hash33(mod(p + glm::vec3(0., 0., 0.), freq));
    glm::vec3 gb = hash33(mod(p + glm::vec3(1., 0., 0.), freq));
    glm::vec3 gc = hash33(mod(p + glm::vec3(0., 1., 0.), freq));
    glm::vec3 gd = hash33(mod(p + glm::vec3(1., 1., 0.), freq));
    glm::vec3 ge = hash33(mod(p + glm::vec3(0., 0., 1.), freq));
    glm::vec3 gf = hash33(mod(p + glm::vec3(1., 0., 1.), freq));
    glm::vec3 gg = hash33(mod(p + glm::vec3(0., 1., 1.), freq));
    glm::vec3 gh = hash33(mod(p + glm::vec3(1., 1., 1.), freq));

    // projections
    float va = dot(ga, w - glm::vec3(0., 0., 0.));
    float vb = dot(gb, w - glm::vec3(1., 0., 0.));
    float vc = dot(gc, w - glm::vec3(0., 1., 0.));
    float vd = dot(gd, w - glm::vec3(1., 1., 0.));
    float ve = dot(ge, w - glm::vec3(0., 0., 1.));
    float vf = dot(gf, w - glm::vec3(1., 0., 1.));
    float vg = dot(gg, w - glm::vec3(0., 1., 1.));
    float vh = dot(gh, w - glm::vec3(1., 1., 1.));

    // interpolation
    return va + u.x * (vb - va) + u.y * (vc - va) + u.z * (ve - va) + u.x * u.y * (va - vb - vc + vd) + u.y * u.z * (va - vc - ve + vg) + u.z * u.x * (va - vb - ve + vf) + u.x * u.y * u.z * (-va + vb + vc - vd + ve - vf - vg + vh);
}

// Tileable 3D worley noise
float WorleyNoise(glm::vec3 uv, float freq)
{
    glm::vec3 id  = glm::floor(uv);
    glm::vec3 p   = glm::fract(uv);
    float minDist = 10000.;
    for (float x = -1.; x <= 1.; ++x) {
        for (float y = -1.; y <= 1.; ++y) {
            for (float z = -1.; z <= 1.; ++z) {
                glm::vec3 offset = glm::vec3(x, y, z);
                glm::vec3 h      = hash33(glm::mod(id + offset, glm::vec3(freq))) * .5f + .5f;
                h += offset;
                glm::vec3 d = p - h;
                minDist     = glm::min(minDist, glm::dot(d, d));
            }
        }
    }

    // inverted worley noise
    return 1. - minDist;
}

// Tileable Worley fbm inspired by Andrew Schneider's Real-Time Volumetric Cloudscapes
// chapter in GPU Pro 7.
float WorleyFbm(glm::vec3 p, float freq)
{
    return WorleyNoise(p * freq, freq) * .625
        + WorleyNoise(p * freq * 2.f, freq * 2.) * .25
        + WorleyNoise(p * freq * 4.f, freq * 4.f) * .125;
}

// Fbm for Perlin noise based on iq's blog
float Perlinfbm(glm::vec3 p, float freq, int octaves)
{
    float G     = exp2(-.85);
    float amp   = 1.;
    float noise = 0.;
    for (int i = 0; i < octaves; ++i) {
        noise += amp * gradientNoise(p * freq, freq);
        freq *= 2.;
        amp *= G;
    }

    return noise;
}

float WorleyPerlinNoise(const glm::vec3& a_UVW, const float& a_Freq)
{
    float perlinNoise = glm::mix(1.f, Perlinfbm(a_UVW, 4.f, 7), .5f);
    perlinNoise       = std::abs(perlinNoise * 2.f - 1.f); // billowy perlin noise
    float worleyNoise = WorleyFbm(a_UVW, a_Freq);
    return remap(perlinNoise, 0., 1., worleyNoise, 1.); // perlin-worley
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
                float noise  = WorleyPerlinNoise({ noiseX, noiseY, noiseZ }, noiseFreq);
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

MSG::ImageInfo GetImageInfo()
{
    return {
        .width     = FOG_DENSITY_WIDTH,
        .height    = FOG_DENSITY_HEIGHT,
        .depth     = FOG_DENSITY_DEPTH,
        .pixelDesc = MSG::PixelSizedFormat::Float16_RGBA
    };
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
    , participatingMediaImage0(GetImageInfo())
    , participatingMediaImage1(GetImageInfo())
    , fogSettingsBuffer(std::make_shared<OGLTypedBuffer<GLSL::FogSettings>>(context))
    , fogCameraBuffer(std::make_shared<OGLTypedBuffer<GLSL::CameraUBO>>(context))
    , noiseTexture(GenerateNoiseTexture(context))
    , sampler(std::make_shared<OGLSampler>(context, GetSamplerParameters()))
    , participatingMediaTexture0(std::make_shared<OGLTexture3D>(context, GetParticipatingMediaTextureInfo()))
    , participatingMediaTexture1(std::make_shared<OGLTexture3D>(context, GetParticipatingMediaTextureInfo()))
{
    participatingMediaImage0.Allocate();
    participatingMediaImage1.Allocate();
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

void MSG::Renderer::VolumetricFog::Update(Renderer::Impl& a_Renderer)
{
    // if (a_Scene.GetVisibleEntities().fogAreas.empty())
    //     return;
    auto& scene                  = *a_Renderer.activeScene;
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

    participatingMediaImage0.Fill(glm::vec4(fogSettings.globalScattering, fogSettings.globalExtinction));
    participatingMediaImage1.Fill(glm::vec4(fogSettings.globalEmissive, fogSettings.globalPhaseG));
    glm::vec3 uvw;
    const auto cameraVP    = cameraUBO.current.projection * cameraUBO.current.view;
    const auto cameraInvVP = glm::inverse(cameraVP);
    auto& registry         = *scene.GetRegistry();
    for (uint32_t z = 0; z < participatingMediaImage0.GetSize().z; z++) {
        uvw.z = z / float(participatingMediaImage0.GetSize().z);
        for (uint32_t y = 0; y < participatingMediaImage0.GetSize().y; y++) {
            uvw.y = y / float(participatingMediaImage0.GetSize().y);
            for (uint32_t x = 0; x < participatingMediaImage0.GetSize().x; x++) {
                uvw.x                 = x / float(participatingMediaImage0.GetSize().x);
                glm::vec3 fogNDC      = GLSL::FogNDCFromUVW(uvw, fogSettings.volumetricFog.depthExp);
                glm::vec4 fogProjPos  = cameraInvVP * glm::vec4(fogNDC, 1.f);
                glm::vec3 fogWorldPos = glm::vec3(fogProjPos) / fogProjPos.w;
                auto fogScatExt       = participatingMediaImage0.Load({ x, y, z });
                auto fogEmisPha       = participatingMediaImage1.Load({ x, y, z });
                for (auto& entity : scene.GetVisibleEntities().fogAreas) {
                    auto& fogAreaTransform = registry.GetComponent<Transform>(entity).GetWorldTransformMatrix();
                    auto& fogArea          = registry.GetComponent<FogArea>(entity);
                    float dist             = fogArea.Distance(fogWorldPos, fogAreaTransform);
                    if (dist <= 0) {
                        float intensity = std::min(pow(-dist, fogArea.GetAttenuationExp()), 1.f);
                        fogScatExt += intensity * glm::vec4(fogArea.GetScattering(), fogArea.GetExtinction());
                        fogEmisPha += intensity * glm::vec4(fogArea.GetEmissive(), fogArea.GetPhaseG());
                    }
                }
                fogEmisPha.a = std::min(fogEmisPha.a, 1.f);
                participatingMediaImage0.Store({ x, y, z }, fogScatExt);
                participatingMediaImage1.Store({ x, y, z }, fogEmisPha);
            }
        }
    }
    participatingMediaTexture0->UploadLevel(0, participatingMediaImage0);
    participatingMediaTexture1->UploadLevel(0, participatingMediaImage1);
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
    resolution             = a_Settings.volumetricFogRes;
    lightInjectionProgram  = a_Renderer.shaderCompiler.CompileProgram("FogLightsInjection");
    integrationProgram     = a_Renderer.shaderCompiler.CompileProgram("FogIntegration");
    scatteringTexture      = std::make_shared<OGLTexture3D>(context, GetIntegrationTextureInfo(resolution));
    resultTexture          = std::make_shared<OGLTexture3D>(context, GetIntegrationTextureInfo(resolution));
    resultTexture_Previous = std::make_shared<OGLTexture3D>(context, GetIntegrationTextureInfo(resolution));
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