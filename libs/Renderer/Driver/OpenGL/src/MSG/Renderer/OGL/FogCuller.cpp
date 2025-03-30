#include <MSG/Camera.hpp>
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

#include <Bindings.glsl>
#include <Fog.glsl>

#include <GL/glew.h>

MSG::OGLTexture3DInfo GetParticipatingMediaTextureInfo()
{
    return {
        .width       = FOG_DENSITY_WIDTH,
        .height      = FOG_DENSITY_HEIGHT,
        .depth       = FOG_DENSITY_DEPTH,
        .sizedFormat = GL_RGBA16F
    };
}

MSG::OGLTexture3DInfo GetIntegrationTextureInfo()
{
    return {
        .width       = FOG_WIDTH,
        .height      = FOG_HEIGHT,
        .depth       = FOG_DEPTH,
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
    perlinNoise       = abs(perlinNoise * 2.f - 1.f); // billowy perlin noise
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

MSG::OGLSamplerParameters GetNoiseSamplerParams()
{
    return {
        .minFilter = GL_LINEAR,
        .magFilter = GL_LINEAR,
        .wrapS     = GL_REPEAT,
        .wrapT     = GL_REPEAT,
        .wrapR     = GL_REPEAT,
    };
}

MSG::Renderer::FogCuller::FogCuller(Renderer::Impl& a_Renderer)
    : context(a_Renderer.context)
    , participatingMediaImage0(GetImageInfo())
    , participatingMediaImage1(GetImageInfo())
    , fogSettingsBuffer(std::make_shared<OGLTypedBuffer<GLSL::FogSettings>>(context))
    , noiseSampler(std::make_shared<OGLSampler>(context, GetNoiseSamplerParams()))
    , noiseTexture(GenerateNoiseTexture(context))
    , participatingMediaTexture0(std::make_shared<OGLTexture3D>(context, GetParticipatingMediaTextureInfo()))
    , participatingMediaTexture1(std::make_shared<OGLTexture3D>(context, GetParticipatingMediaTextureInfo()))
    , scatteringTexture(std::make_shared<OGLTexture3D>(context, GetIntegrationTextureInfo()))
    , resultTexture(std::make_shared<OGLTexture3D>(context, GetIntegrationTextureInfo()))
    , lightInjectionProgram(a_Renderer.shaderCompiler.CompileProgram("FogLightsInjection"))
    , integrationProgram(a_Renderer.shaderCompiler.CompileProgram("FogIntegration"))
{
    participatingMediaImage0.Allocate();
    participatingMediaImage1.Allocate();
    renderPassInfo.name = "FogPass";
}

void MSG::Renderer::FogCuller::Update(const Scene& a_Scene)
{
    if (a_Scene.GetVisibleEntities().fogAreas.empty())
        return;
    GLSL::FogSettings fogSettings {
        .noiseDensityOffset    = a_Scene.GetFogSettings().noiseDensityOffset,
        .noiseDensityScale     = a_Scene.GetFogSettings().noiseDensityScale,
        .noiseDensityIntensity = a_Scene.GetFogSettings().noiseDensityIntensity,
        .multiplier            = a_Scene.GetFogSettings().multiplier,
        .transmittanceExp      = a_Scene.GetFogSettings().transmittanceExp,
    };
    fogSettingsBuffer->Set(fogSettings);
    fogSettingsBuffer->Update();
    auto& registry                 = *a_Scene.GetRegistry();
    auto const& camera             = a_Scene.GetCamera().GetComponent<Camera>();
    auto const& cameraTransform    = a_Scene.GetCamera().GetComponent<Transform>();
    auto const& cameraProjection   = camera.projection.GetMatrix();
    auto const cameraView          = glm::inverse(cameraTransform.GetWorldTransformMatrix());
    auto const cameraVP            = cameraProjection * cameraView;
    glm::vec4 scatteringExtinction = { 0, 0, 0, 0 };
    glm::vec4 emissivePhase        = { 0, 0, 0, 0 };
    for (auto& entity : a_Scene.GetVisibleEntities().fogAreas) {
        auto& fogArea = registry.GetComponent<FogArea>(entity);
        auto& fogBV   = registry.GetComponent<BoundingVolume>(entity);
        if (glm::any(glm::isinf(fogArea.GetHalfSize()))) {
            assert(fogArea.GetScatteringExtinction().GetSize() == glm::uvec3(1) && "Infinite fog sources can only have one color.");
            if (fogArea.GetFogAreaMode() == FogAreaMode::Add) {
                scatteringExtinction += fogArea.GetScatteringExtinction().Load({ 0, 0, 0 });
                emissivePhase.a += fogArea.GetPhaseG();
            }
        }
    }
    participatingMediaImage0.Fill(scatteringExtinction);
    participatingMediaImage1.Fill(emissivePhase);
    glm::vec3 uvz;
    // for (uint32_t z = 0; z < image.GetSize().z; z++) {
    //     uvz.z = z / float(image.GetSize().z);
    //     for (uint32_t y = 0; y < image.GetSize().y; y++) {
    //         uvz.y = y / float(image.GetSize().y);
    //         for (uint32_t x = 0; x < image.GetSize().x; x++) {
    //             uvz.z = z / float(image.GetSize().z);
    //             for (auto& entity : a_Scene.GetVisibleEntities().fogAreas) {
    //                 auto& fogArea = registry.GetComponent<FogArea>(entity);
    //                 if (glm::any(glm::isinf(fogArea.GetHalfSize())))
    //                     continue;
    //                 auto& fogBV    = registry.GetComponent<BoundingVolume>(entity);
    //                 auto fogProjBV = cameraVP * fogBV;
    //                 glm::vec4 fogColor;
    //                 image.Store({ x, y, z }, fogColor);
    //             }
    //         }
    //     }
    // }
    participatingMediaTexture0->UploadLevel(0, participatingMediaImage0);
    participatingMediaTexture1->UploadLevel(0, participatingMediaImage1);
}

MSG::OGLRenderPass* MSG::Renderer::FogCuller::GetComputePass(
    const LightCullerFwd& a_LightCuller,
    const std::shared_ptr<OGLSampler>& a_ShadowSampler,
    const std::shared_ptr<OGLBuffer>& a_CameraBuffer,
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
            .texture = participatingMediaTexture0
        };
        cp.bindings.textures.at(1) = {
            .texture = participatingMediaTexture1
        };
        for (auto i = 0u; i < a_LightCuller.shadows.buffer->Get().count; i++) {
            cp.bindings.textures.at(SAMPLERS_FWD_SHADOW + i) = OGLTextureBindingInfo {
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
        cp.bindings.uniformBuffers.at(UBO_CAMERA) = {
            .buffer = a_CameraBuffer,
            .offset = 0,
            .size   = a_CameraBuffer->size
        };
        cp.bindings.uniformBuffers.at(UBO_FOG_SETTINGS) = {
            .buffer = fogSettingsBuffer,
            .offset = 0,
            .size   = fogSettingsBuffer->size
        };
        cp.shaderState.program = lightInjectionProgram;
        cp.memoryBarrier       = GL_TEXTURE_UPDATE_BARRIER_BIT;
        cp.workgroupX          = FOG_WORKGROUPS;
        cp.workgroupY          = FOG_WORKGROUPS;
        cp.workgroupZ          = FOG_WORKGROUPS;
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
            .texture = scatteringTexture
        };
        cp.bindings.textures.at(1) = {
            .texture = noiseTexture
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
        cp.bindings.uniformBuffers.at(UBO_FOG_SETTINGS) = {
            .buffer = fogSettingsBuffer,
            .offset = 0,
            .size   = fogSettingsBuffer->size
        };
        cp.shaderState.program = integrationProgram;
        cp.memoryBarrier       = GL_TEXTURE_UPDATE_BARRIER_BIT;
        cp.workgroupX          = FOG_WORKGROUPS;
        cp.workgroupY          = FOG_WORKGROUPS;
        cp.workgroupZ          = FOG_WORKGROUPS;
    }
    return new OGLRenderPass(renderPassInfo);
}