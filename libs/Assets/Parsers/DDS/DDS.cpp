#include <MSG/Assets/Asset.hpp>
#include <MSG/Debug.hpp>
#include <MSG/Image.hpp>
#include <MSG/ImageUtils.hpp>
#include <MSG/Texture.hpp>

#include <DXGIFormatToMsg.hpp>
#include <DirectXTex.h>

#include <bitset>
#include <fstream>
#include <strstream>

namespace Msg::Assets {
static std::shared_ptr<Asset> ParseDDSFromStream(const std::shared_ptr<Asset>& a_Container, std::istream& a_Stream)
{
    DirectX::ScratchImage srcImage;
    {
        a_Stream.seekg(0, std::ios::end);
        auto const file_size = a_Stream.tellg();
        a_Stream.seekg(0, std::ios::beg);
        std::vector<std::byte> rawData(file_size);
        a_Stream.read(reinterpret_cast<char*>(rawData.data()), file_size);
        DirectX::LoadFromDDSMemory(rawData.data(), rawData.size(), DirectX::DDS_FLAGS_FORCE_RGB, nullptr, srcImage);
    }

    auto& metadata       = srcImage.GetMetadata();
    bool needsConversion = ToMsg(metadata.format) == PixelSizedFormat::Unknown;
    bool isBC1ToBC5      = metadata.format >= DXGI_FORMAT_BC1_TYPELESS && metadata.format <= DXGI_FORMAT_BC5_SNORM;
    bool isBC6ToBC7      = metadata.format >= DXGI_FORMAT_BC6H_TYPELESS && metadata.format <= DXGI_FORMAT_BC7_UNORM_SRGB;
    bool isCompressed    = isBC1ToBC5 || isBC6ToBC7;

    Msg::Texture texture;
    glm::uvec3 imageSize { metadata.width, metadata.height, metadata.depth };
    if (needsConversion) {
        if (isCompressed)
            texture.SetPixelDescriptor(PixelSizedFormat::DXT5_RGBA);
        else
            texture.SetPixelDescriptor(PixelSizedFormat::Uint8_NormalizedRGBA);
    }
    for (uint32_t mip = 0; mip < metadata.mipLevels; mip++) {
        DirectX::Image image = *srcImage.GetImage(mip, 0, 0);
        std::vector<std::byte> data;
        if (needsConversion) {
            DirectX::ScratchImage convImage;
            PixelSizedFormat textureFormat;
            if (isCompressed) {
                DirectX::ScratchImage decompImage;
                DirectX::Decompress(image, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM, decompImage);
                DirectX::Compress(*decompImage.GetImage(0, 0, 0),
                    DXGI_FORMAT::DXGI_FORMAT_BC3_UNORM,
                    DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT,
                    convImage);
            } else {
                DirectX::Convert(image,
                    DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
                    DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT,
                    convImage);
            }
            auto pixels    = (std::byte*)convImage.GetPixels();
            auto pixelSize = convImage.GetPixelsSize();
            data           = { pixels, pixels + pixelSize };
        } else {
            auto pixels    = (std::byte*)image.pixels;
            auto pixelSize = image.slicePitch;
            data           = { pixels, pixels + pixelSize };
        }
        Msg::ImageInfo imageInfo;
        imageInfo.width     = imageSize.x;
        imageInfo.height    = imageSize.y;
        imageInfo.depth     = imageSize.z;
        imageInfo.storage   = data;
        imageInfo.pixelDesc = texture.GetPixelDescriptor();
        texture.emplace_back(std::make_shared<Image>(imageInfo));
        imageSize /= 2u;
    }
    a_Container->AddObject(std::make_shared<Texture>(texture));
    a_Container->SetLoaded(true);
    return a_Container;
}

static std::shared_ptr<Asset> ParseDDSFromFile(const std::shared_ptr<Asset>& a_Container)
{
    std::ifstream stream(a_Container->GetUri().DecodePath(), std::ios_base::binary);
    return ParseDDSFromStream(a_Container, stream);
}

static std::shared_ptr<Asset> ParseDDSFromBinary(const std::shared_ptr<Asset>& a_Container)
{
    std::vector<std::byte> binary = DataUri(a_Container->GetUri()).Decode();
    auto stream                   = std::istrstream(reinterpret_cast<const char*>(binary.data()), binary.size());
    return a_Container;
}
std::shared_ptr<Asset> ParseDDSImage(const std::shared_ptr<Asset>& a_Container)
{
    auto& uri = a_Container->GetUri();
    if (uri.GetScheme() == "data")
        return ParseDDSFromBinary(a_Container);
    else
        return ParseDDSFromFile(a_Container);
    return a_Container;
}
}
