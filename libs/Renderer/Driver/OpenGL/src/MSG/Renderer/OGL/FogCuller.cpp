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

// #define hash(p) glm::fract(sin(dot(p, glm::vec2(11.9898, 78.233))) * 43758.5453) // iq suggestion, for Windows

// float B(glm::vec2 U)
// {
//     float v = 0.;
//     for (int k = 0; k < 9; k++)
//         v += hash(U + glm::vec2(k % 3 - 1, k / 3 - 1));
//     return .9 * (1.125 * hash(U) - v / 8.) + .5;
// }

static const uint8_t perm[256] = {
    151, 160, 137, 91, 90, 15,
    131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
    190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
    77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
    102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
    135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
    5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
    223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
    129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
    251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
    49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

static inline uint8_t hash(int32_t i)
{
    return perm[static_cast<uint8_t>(i)];
}

static inline int32_t fastfloor(float fp)
{
    int32_t i = static_cast<int32_t>(fp);
    return (fp < i) ? (i - 1) : (i);
}

static float grad(int32_t hash, float x, float y, float z)
{
    int h   = hash & 15; // Convert low 4 bits of hash code into 12 simple
    float u = h < 8 ? x : y; // gradient directions, and compute dot product.
    float v = h < 4 ? y : h == 12 || h == 14 ? x
                                             : z; // Fix repeats at h = 12 to 15
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

float PerlinNoise(float x, float y, float z)
{
    float n0, n1, n2, n3; // Noise contributions from the four corners

    // Skewing/Unskewing factors for 3D
    static const float F3 = 1.0f / 3.0f;
    static const float G3 = 1.0f / 6.0f;

    // Skew the input space to determine which simplex cell we're in
    float s  = (x + y + z) * F3; // Very nice and simple skew factor for 3D
    int i    = fastfloor(x + s);
    int j    = fastfloor(y + s);
    int k    = fastfloor(z + s);
    float t  = (i + j + k) * G3;
    float X0 = i - t; // Unskew the cell origin back to (x,y,z) space
    float Y0 = j - t;
    float Z0 = k - t;
    float x0 = x - X0; // The x,y,z distances from the cell origin
    float y0 = y - Y0;
    float z0 = z - Z0;

    // For the 3D case, the simplex shape is a slightly irregular tetrahedron.
    // Determine which simplex we are in.
    int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
    int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords
    if (x0 >= y0) {
        if (y0 >= z0) {
            i1 = 1;
            j1 = 0;
            k1 = 0;
            i2 = 1;
            j2 = 1;
            k2 = 0; // X Y Z order
        } else if (x0 >= z0) {
            i1 = 1;
            j1 = 0;
            k1 = 0;
            i2 = 1;
            j2 = 0;
            k2 = 1; // X Z Y order
        } else {
            i1 = 0;
            j1 = 0;
            k1 = 1;
            i2 = 1;
            j2 = 0;
            k2 = 1; // Z X Y order
        }
    } else { // x0<y0
        if (y0 < z0) {
            i1 = 0;
            j1 = 0;
            k1 = 1;
            i2 = 0;
            j2 = 1;
            k2 = 1; // Z Y X order
        } else if (x0 < z0) {
            i1 = 0;
            j1 = 1;
            k1 = 0;
            i2 = 0;
            j2 = 1;
            k2 = 1; // Y Z X order
        } else {
            i1 = 0;
            j1 = 1;
            k1 = 0;
            i2 = 1;
            j2 = 1;
            k2 = 0; // Y X Z order
        }
    }

    // A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
    // a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
    // a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
    // c = 1/6.
    float x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
    float y1 = y0 - j1 + G3;
    float z1 = z0 - k1 + G3;
    float x2 = x0 - i2 + 2.0f * G3; // Offsets for third corner in (x,y,z) coords
    float y2 = y0 - j2 + 2.0f * G3;
    float z2 = z0 - k2 + 2.0f * G3;
    float x3 = x0 - 1.0f + 3.0f * G3; // Offsets for last corner in (x,y,z) coords
    float y3 = y0 - 1.0f + 3.0f * G3;
    float z3 = z0 - 1.0f + 3.0f * G3;

    // Work out the hashed gradient indices of the four simplex corners
    int gi0 = hash(i + hash(j + hash(k)));
    int gi1 = hash(i + i1 + hash(j + j1 + hash(k + k1)));
    int gi2 = hash(i + i2 + hash(j + j2 + hash(k + k2)));
    int gi3 = hash(i + 1 + hash(j + 1 + hash(k + 1)));

    // Calculate the contribution from the four corners
    float t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;
    if (t0 < 0) {
        n0 = 0.0;
    } else {
        t0 *= t0;
        n0 = t0 * t0 * grad(gi0, x0, y0, z0);
    }
    float t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
    if (t1 < 0) {
        n1 = 0.0;
    } else {
        t1 *= t1;
        n1 = t1 * t1 * grad(gi1, x1, y1, z1);
    }
    float t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;
    if (t2 < 0) {
        n2 = 0.0;
    } else {
        t2 *= t2;
        n2 = t2 * t2 * grad(gi2, x2, y2, z2);
    }
    float t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;
    if (t3 < 0) {
        n3 = 0.0;
    } else {
        t3 *= t3;
        n3 = t3 * t3 * grad(gi3, x3, y3, z3);
    }
    // Add contributions from each corner to get the final noise value.
    // The result is scaled to stay just inside [-1,1]
    return 32.0f * (n0 + n1 + n2 + n3);
}

std::shared_ptr<MSG::OGLTexture3D> GenerateNoiseTexture(MSG::OGLContext& a_Ctx)
{
    MSG::Image image({
        .width     = 32,
        .height    = 32,
        .depth     = 32,
        .pixelDesc = MSG::PixelSizedFormat::Uint8_NormalizedR,
    });
    image.Allocate();
    for (uint32_t z = 0; z < image.GetSize().z; z++) {
        for (uint32_t y = 0; y < image.GetSize().y; y++) {
            for (uint32_t x = 0; x < image.GetSize().x; x++) {
                float noise = PerlinNoise(x, y, z);
                image.Store({ x, y, z }, glm::vec4 { noise });
            }
        }
    }
    auto texture = std::make_shared<MSG::OGLTexture3D>(a_Ctx,
        MSG::OGLTexture3DInfo {
            .width       = 32,
            .height      = 32,
            .depth       = 32,
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
        .wrapS     = GL_MIRRORED_REPEAT,
        .wrapT     = GL_MIRRORED_REPEAT,
        .wrapR     = GL_MIRRORED_REPEAT,
    };
}

MSG::Renderer::FogCuller::FogCuller(Renderer::Impl& a_Renderer)
    : context(a_Renderer.context)
    , image(GetImageInfo())
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
    cp.workgroupX          = FOG_WIDTH;
    cp.workgroupY          = FOG_HEIGHT;
    return new OGLRenderPass(renderPassInfo);
}