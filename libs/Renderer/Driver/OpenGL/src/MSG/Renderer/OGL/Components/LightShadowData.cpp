#include <MSG/Camera/Projection.hpp>
#include <MSG/Entity/Node.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/OGLBindlessTextureSampler.hpp>
#include <MSG/OGLFrameBuffer.hpp>
#include <MSG/OGLTexture2DArray.hpp>
#include <MSG/OGLTextureCube.hpp>
#include <MSG/Renderer/OGL/Components/LightShadowData.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Scene.hpp>
#include <MSG/SphericalHarmonics.hpp>
#include <MSG/Texture.hpp>

#include <GL/glew.h>

namespace Msg::Renderer {
GLenum GetShadowDepthPixelFormat(const LightShadowPrecision& a_Precision)
{
    switch (a_Precision) {
    case LightShadowPrecision::High:
        return GL_DEPTH_COMPONENT32;
    case LightShadowPrecision::Medium:
        return GL_DEPTH_COMPONENT24;
    case LightShadowPrecision::Low:
        return GL_DEPTH_COMPONENT16;
    }
    return GL_NONE;
}

std::shared_ptr<OGLTexture> CreateTextureDepth(OGLContext& a_Ctx, const LightShadowSettings& a_ShadowSettings, const size_t a_ViewportsCount)
{
    OGLTexture2DArrayInfo info {
        .width       = a_ShadowSettings.resolution,
        .height      = a_ShadowSettings.resolution,
        .layers      = uint32_t(a_ViewportsCount),
        .sizedFormat = GetShadowDepthPixelFormat(a_ShadowSettings.precision)
    };
    return std::make_shared<OGLTexture2DArray>(a_Ctx, info);
}

Msg::Renderer::LightShadowData::LightShadowData(Renderer::Impl& a_Rdr)
    : bufferDepthRange(std::make_shared<OGLTypedBufferArray<float>>(a_Rdr.context, 4))
    , bufferDepthRange_Prev(std::make_shared<OGLTypedBufferArray<float>>(a_Rdr.context, 4))
{
    bufferDepthRange->Set(0, minDepth);
    bufferDepthRange->Set(1, maxDepth);
    bufferDepthRange->Update();
    bufferDepthRange_Prev->Set(0, minDepth);
    bufferDepthRange_Prev->Set(1, maxDepth);
    bufferDepthRange_Prev->Update();
}

void LightShadowData::Update(Renderer::Impl& a_Rdr,
    const std::shared_ptr<OGLSampler>& a_Sampler,
    const LightShadowSettings& a_ShadowSettings,
    const size_t& a_ViewportCount)
{
    if (textureSampler == nullptr || textureSampler->texture->height != a_ShadowSettings.resolution)
        _UpdateTextureSampler(a_Rdr, a_Sampler, a_ShadowSettings, a_ViewportCount);
    UpdateDepthRange();
}
}

void Msg::Renderer::LightShadowData::UpdateDepthRange()
{
    bufferDepthRange->Read();
    assert(bufferDepthRange->Get(0) != std::numeric_limits<float>::quiet_NaN());
    assert(bufferDepthRange->Get(1) != std::numeric_limits<float>::quiet_NaN());
    // use rolling average to avoid sudden jumps
    minDepth = glm::mix(bufferDepthRange->Get(0), minDepth, 0.95);
    maxDepth = glm::mix(bufferDepthRange->Get(1), maxDepth, 0.95);
    bufferDepthRange->Set(0, minDepth);
    bufferDepthRange->Set(1, maxDepth);
    bufferDepthRange->Update();
    std::swap(bufferDepthRange, bufferDepthRange_Prev);
    // reset range for next frame
    bufferDepthRange->Set(0, 100000);
    bufferDepthRange->Set(1, 0);
    bufferDepthRange->Update();
}

void Msg::Renderer::LightShadowData::_UpdateTextureSampler(Renderer::Impl& a_Rdr,
    const std::shared_ptr<OGLSampler>& a_Sampler,
    const LightShadowSettings& a_ShadowSettings,
    const size_t& a_ViewportCount)
{
    auto textureDepth = CreateTextureDepth(a_Rdr.context, a_ShadowSettings, a_ViewportCount);
    textureSampler    = std::make_shared<OGLBindlessTextureSampler>(a_Rdr.context, textureDepth, a_Sampler);
    minDepth          = 0;
    maxDepth          = 1;
    frameBuffers.resize(a_ViewportCount);
    for (uint8_t layer = 0u; layer < textureDepth->depth; layer++) {
        frameBuffers.at(layer) = std::make_shared<OGLFrameBuffer>(a_Rdr.context,
            OGLFrameBufferCreateInfo {
                .layered     = textureDepth->depth > 1 ? true : false,
                .defaultSize = { a_ShadowSettings.resolution, a_ShadowSettings.resolution, 1 },
                .depthBuffer = { .layer = layer, .texture = textureDepth },
            });
    }
}