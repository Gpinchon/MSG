#include <MSG/Debug.hpp>
#include <MSG/ImageUtils.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/Texture.hpp>
#include <MSG/TextureUtils.hpp>
#include <MSG/ThreadPool.hpp>

#include <cmath>

#include <glm/common.hpp>
#include <glm/vec2.hpp>

namespace Msg {
template <typename T>
uint32_t GetMipCount(const T& a_BaseSize)
{
    typename T::value_type maxVal = 0;
    for (uint8_t i = 0; i < T::length(); i++)
        maxVal = glm::max(maxVal, a_BaseSize[i]);
    return maxVal == 0 ? 0 : floor(log2(maxVal) + 1);
}

template <uint8_t Dimension>
auto CreateMip(const PixelDescriptor& a_PD, const glm::ivec3& a_BaseSize, const glm::ivec3& a_LevelSize)
{
    glm::uvec3 size = a_BaseSize;
    for (uint8_t i = 0; i < Dimension; ++i)
        size[i] = a_LevelSize[i];
    return std::make_shared<Image>(ImageInfo {
        .width     = size.x,
        .height    = size.y,
        .depth     = size.z,
        .pixelDesc = a_PD,
    });
}

template <typename SamplerType>
void LvlGenFunc(ThreadPool& a_Tp, const SamplerType& a_Sampler, const uint32_t& a_LvlIndex, const Image& a_Src, Image& a_Dst)
{
    auto dstSize = a_Dst.GetSize();
    auto tcMax   = glm::max(glm::vec3(dstSize - 1u), 1.f);
    for (uint32_t z = 0; z < dstSize.z; z++) {
        float w = float(z + 0.25f) / tcMax.z; // + 0.25f and not 0.5f because (no idea)...
        for (uint32_t y = 0; y < dstSize.y; y++) {
            float v = float(y + 0.25f) / tcMax.y;
            for (uint32_t x = 0; x < dstSize.x; x++) {
                float u = float(x + 0.25f) / tcMax.x;
                a_Dst.Store({ x, y, z }, a_Sampler.Sample(a_Src, glm::vec3(u, v, w)));
            }
        }
    }
}

template <>
void LvlGenFunc(ThreadPool& a_Tp, const SamplerCube& a_Sampler, const uint32_t& a_LvlIndex, const Image& a_Src, Image& a_Dst)
{
    auto tcMax = glm::vec2(a_Dst.GetSize() - 1u);
    for (auto side = 0u; side < 6; side++) {
        a_Tp.PushCommand([&a_Sampler, &a_Src, &a_Dst, tcMax, side]() mutable {
            for (uint32_t y = 0u; y < a_Dst.GetSize().y; y++) {
                auto v = (y + 0.25f) / tcMax.y;
                for (uint32_t x = 0u; x < a_Dst.GetSize().x; x++) {
                    auto u     = (x + 0.25f) / tcMax.x;
                    auto dir   = CubemapUVWToSampleDir({ u, v, side });
                    auto color = a_Sampler.Sample(a_Src, dir);
                    a_Dst.Store({ x, y, side }, color);
                }
            }
        },
            false);
    }
    a_Tp.Wait();
}

template <uint8_t Dimension, typename SamplerType>
void GenerateMipMaps(Texture& a_Texture, const SamplerType& a_Sampler = {})
{
    ThreadPool threadPool;
    const auto pixelDesc = a_Texture.GetPixelDescriptor();
    const auto baseSize  = glm::ivec3(a_Texture.GetSize());
    const auto mipNbr    = GetMipCount(baseSize);
    auto srcLevel        = a_Texture.front();
    srcLevel->Map();
    a_Texture.resize(mipNbr);
    for (auto level = 1u; level < mipNbr; level++) {
        auto levelSize      = glm::max(baseSize / int(pow(2, level)), 1);
        auto mip            = CreateMip<Dimension>(pixelDesc, baseSize, levelSize);
        a_Texture.at(level) = mip;
        mip->Allocate();
        ImageClear(*mip);
        mip->Map();
        LvlGenFunc(threadPool, a_Sampler, level, *srcLevel, *mip);
        srcLevel->Unmap();
        srcLevel = mip;
    }
    srcLevel->Unmap();
}
}

void Msg::TextureGenerateMipmaps(Texture& a_Dst)
{
    Sampler samplerSettings;
    samplerSettings.SetMinFilter(SamplerFilter::LinearMipmapLinear);
    if (a_Dst.GetType() == TextureType::Texture1D)
        GenerateMipMaps<1, Sampler1D>(a_Dst, samplerSettings);
    else if (a_Dst.GetType() == TextureType::Texture2D)
        GenerateMipMaps<2, Sampler2D>(a_Dst, samplerSettings);
    else if (a_Dst.GetType() == TextureType::Texture3D)
        GenerateMipMaps<3, Sampler3D>(a_Dst, samplerSettings);
    else if (a_Dst.GetType() == TextureType::TextureCubemap)
        GenerateMipMaps<2, SamplerCube>(a_Dst, samplerSettings);
    else
        MSGErrorLog("Mipmap generation not implemented for this texture type yet");
}

void Msg::TextureCompress(Texture& a_Dst)
{
    if (a_Dst.GetPixelDescriptor().GetSizedFormat() == PixelSizedFormat::DXT5_RGBA) {
        MSGDebugLog("Texture already compressed");
        return;
    }

    TextureBase result;
    result.reserve(a_Dst.size());
    for (auto& level : a_Dst)
        result.emplace_back(std::make_shared<Image>(ImageCompress(*level)));
    result.shrink_to_fit();
    a_Dst = result;
    a_Dst.SetSize(a_Dst.front()->GetSize());
    a_Dst.SetPixelDescriptor(a_Dst.front()->GetPixelDescriptor());
}

void Msg::TextureDecompress(Texture& a_Dst)
{
    if (a_Dst.GetPixelDescriptor().GetSizedFormat() != PixelSizedFormat::DXT5_RGBA) {
        MSGDebugLog("Texture is not compressed");
        return;
    }

    TextureBase result;
    result.reserve(a_Dst.size());
    for (auto& level : a_Dst)
        result.emplace_back(std::make_shared<Image>(ImageDecompress(*level)));
    result.shrink_to_fit();
    a_Dst = result;
    a_Dst.SetSize(a_Dst.front()->GetSize());
    a_Dst.SetPixelDescriptor(a_Dst.front()->GetPixelDescriptor());
}

Msg::Texture Msg::TextureConvert(const Texture& a_Src, const PixelDescriptor& a_PixelDesc)
{
    Texture result(
        a_Src.GetType(),
        a_PixelDesc,
        a_Src.GetSize());
    for (uint32_t lvl = 0; lvl < result.size(); lvl++) {
        auto& srcLvl = *a_Src.at(lvl);
        auto& dstLvl = *result.at(lvl);
        ImageBlit(
            srcLvl, dstLvl,
            glm::uvec3(0), glm::uvec3(0),
            srcLvl.GetSize());
    }
    return result;
}

Msg::Texture Msg::TextureResize(const Texture& a_Src, const glm::uvec3& a_NewSize)
{
    Texture result(
        a_Src.GetType(),
        a_Src.GetPixelDescriptor(),
        a_NewSize);
    for (uint32_t lvl = 0; lvl < result.size(); lvl++) {
        auto& srcLvl       = *a_Src.at(lvl);
        auto& dstLvl       = *result.at(lvl);
        glm::uvec3 lvlSize = glm::max(glm::uvec3(1u), a_NewSize / uint32_t(exp2(lvl)));
        dstLvl             = ImageResize(srcLvl, lvlSize);
    }
    return result;
}

bool Msg::Texture::GetCompressed() const
{
    return GetPixelDescriptor().GetSizedFormat() == PixelSizedFormat::DXT5_RGBA;
}
