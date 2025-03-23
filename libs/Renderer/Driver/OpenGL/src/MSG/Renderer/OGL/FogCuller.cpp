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
#include <Fog.glsl>

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
        .pixelDesc = MSG::PixelSizedFormat::Uint8_NormalizedRGBA
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
    , image(GetImageInfo())
    , fogSettingsBuffer(std::make_shared<OGLTypedBuffer<GLSL::FogSettings>>(context))
    , noiseSampler(std::make_shared<OGLSampler>(context, GetNoiseSamplerParams()))
    , noiseTexture(GenerateNoiseTexture(context))
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
    GLSL::FogSettings fogSettings {
        .noiseDensityOffset    = a_Scene.GetFogSettings().noiseDensityOffset,
        .noiseDensityIntensity = a_Scene.GetFogSettings().noiseDensityIntensity,
        .noiseDensityScale     = a_Scene.GetFogSettings().noiseDensityScale,
        .noiseDepthMultiplier  = a_Scene.GetFogSettings().noiseDepthMultiplier,
        .multiplier            = a_Scene.GetFogSettings().multiplier,
        .attenuationExp        = a_Scene.GetFogSettings().attenuationExp,
    };
    fogSettingsBuffer->Set(fogSettings);
    fogSettingsBuffer->Update();
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
        .layered = true,
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
    cp.shaderState.program = cullingProgram;
    cp.memoryBarrier       = GL_TEXTURE_UPDATE_BARRIER_BIT;
    cp.workgroupX          = FOG_WORKGROUPS;
    cp.workgroupY          = FOG_WORKGROUPS;
    cp.workgroupZ          = FOG_WORKGROUPS;
    return new OGLRenderPass(renderPassInfo);
}
