#include <SG/Core/Image/Cubemap.hpp>
#include <SG/Core/Texture/Texture.hpp>

#include <cmath>

#include <glm/common.hpp>
#include <glm/vec2.hpp>

#define MIPMAPNBR2D(size) int((size.x <= 0 && size.y <= 0) ? 0 : floor(log2(std::max(size.x, size.y))))

namespace TabGraph::SG {
void GenerateCubemapMipMaps(Texture& a_Texture)
{
    std::vector<std::shared_ptr<SG::Image>> mipmaps;
    const auto pixelDesc      = a_Texture.GetPixelDescription();
    const glm::ivec2 baseSize = a_Texture.GetSize();
    const auto mipNbr         = MIPMAPNBR2D(baseSize);
    const auto baseLevel      = std::static_pointer_cast<SG::Cubemap>(a_Texture[0]);
    mipmaps.reserve(mipNbr);
    auto levelSrc = baseLevel;
    for (auto level = 1; level < mipNbr; level++) {
        auto levelSize = glm::max(baseSize / int(pow(2, level)), 1);
        auto mip       = std::make_shared<SG::Cubemap>(pixelDesc, levelSize.x, levelSize.y);
        mip->Allocate();
        mipmaps.emplace_back(mip);
        for (auto side = 0; side < mip->GetSize().z; side++) {
            auto& sideSrc = levelSrc->at(side);
            auto& sideDst = mip->at(side);
            for (auto y = 0; y < mip->GetSize().y; y++) {
                const auto yCoord = y / float(mip->GetSize().y);
                for (auto x = 0; x < mip->GetSize().x; x++) {
                    const auto xCoord = x / float(mip->GetSize().x);
                    const auto color  = sideSrc.LoadNorm({ xCoord, yCoord, 0 }, SG::ImageFilter::Bilinear);
                    sideDst.Store({ x, y, 0 }, color);
                }
            }
        }
        levelSrc = mip;
    }
    a_Texture = mipmaps;
}

void Generate2DMipMaps(Texture& a_Texture)
{
    std::vector<std::shared_ptr<SG::Image>> mipmaps;
    const auto pixelDesc      = a_Texture.GetPixelDescription();
    const glm::ivec2 baseSize = a_Texture.GetSize();
    const auto mipNbr         = MIPMAPNBR2D(baseSize);
    const auto& baseLevel     = *std::static_pointer_cast<SG::Image2D>(mipmaps.front());
    mipmaps.reserve(mipNbr);
    for (auto level = 1; level <= mipNbr; level++) {
        auto levelSize = glm::max(baseSize / int(pow(2, level)), 1);
        auto mip       = std::make_shared<SG::Image2D>(pixelDesc, levelSize.x, levelSize.y);
        mipmaps.emplace_back(mip);
        mip->Allocate();
        for (auto y = 0; y < mip->GetSize().y; y++) {
            const auto yCoord = y / float(mip->GetSize().y);
            for (auto x = 0; x < mip->GetSize().x; x++) {
                const auto xCoord = x / float(mip->GetSize().x);
                const auto color  = baseLevel.LoadNorm({ xCoord, yCoord, 0 }, ImageFilter::Bilinear);
                mip->Store({ x, y, 0 }, color);
            }
        }
    }
    a_Texture = mipmaps;
}

void Texture::GenerateMipmaps()
{
    if (GetType() == TextureType::TextureCubemap)
        GenerateCubemapMipMaps(*this);
    else if (GetType() == TextureType::Texture2D)
        Generate2DMipMaps(*this);
}
}