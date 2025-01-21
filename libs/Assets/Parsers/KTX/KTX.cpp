/*
 * @Author: gpinchon
 * @Date:   2019-02-22 16:13:28
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-01-11 08:46:17
 */

#include <Assets/Asset.hpp>
#include <Core/Buffer/Buffer.hpp>
#include <Core/Buffer/View.hpp>
#include <Core/DataType.hpp>
#include <Core/Image/Cubemap.hpp>
#include <Core/Image/Image1D.hpp>
#include <Core/Image/Image2D.hpp>
#include <Core/Image/Image3D.hpp>
#include <Core/Image/Pixel.hpp>
#include <Core/Texture/Texture.hpp>

#include <algorithm>
#include <fstream>
#include <strstream>

#include <glm/common.hpp>

#define NOMSG
#include <GL/glcorearb.h>

namespace MSG::Assets {
namespace KTX {
    Core::DataType GetPixelType(const uint32_t& a_Type)
    {
        using enum MSG::Core::DataType;
        switch (a_Type) {
        case GL_UNSIGNED_BYTE:
            return Uint8;
        case GL_BYTE:
            return Int8;
        case GL_UNSIGNED_SHORT:
            return Uint16;
        case GL_SHORT:
            return Int16;
        case GL_UNSIGNED_INT:
            return Uint32;
        case GL_INT:
            return Int32;
        case GL_HALF_FLOAT:
            return Float16;
        case GL_FLOAT:
            return Float32;
        default:
            break;
        }
        return Unknown;
    }

    Core::Pixel::UnsizedFormat GetPixelFormat(const uint32_t& a_Format)
    {
        using enum Core::Pixel::UnsizedFormat;
        switch (a_Format) {
        case GL_RED:
            return R;
        case GL_RG:
            return RG;
        case GL_RGB:
            return RGB;
        case GL_RGBA:
            return RGBA;
        case GL_RED_INTEGER:
            return R_Integer;
        case GL_RG_INTEGER:
            return RG_Integer;
        case GL_RGB_INTEGER:
            return RGB_Integer;
        case GL_RGBA_INTEGER:
            return RGBA_Integer;
        case GL_DEPTH_COMPONENT:
            return Depth;
        case GL_STENCIL:
            return Stencil;
        case GL_DEPTH_STENCIL:
            return Depth_Stencil;
        default:
            break;
        }
        return Unknown;
    }

    template <typename T>
    T ReadFromFile(std::istream& a_Stream)
    {
        T data {};
        a_Stream.read((char*)&data, sizeof(T));
        return data;
    }

    template <typename T>
    std::vector<T> ReadVectorFromFile(std::istream& a_Stream, const size_t& a_Count)
    {
        std::vector<T> data(a_Count);
        a_Stream.read((char*)data.data(), sizeof(T) * a_Count);
        return data;
    }

    struct Header {
        uint8_t identifier[12];
        uint32_t endianness;
        uint32_t glType;
        uint32_t glTypeSize;
        uint32_t glFormat;
        uint32_t glInternalFormat;
        uint32_t glBaseInternalFormat;
        glm::uvec3 pixelSize;
        uint32_t numberOfArrayElements;
        uint32_t numberOfFaces;
        uint32_t numberOfMipmapLevels;
        uint32_t bytesOfKeyValueData;
    };

    struct KeyAndValue {
        std::string key;
        std::vector<uint8_t> value;
    };

    std::shared_ptr<Asset> ParseFromStream(const std::shared_ptr<Asset>& a_Container, std::istream& a_Stream)
    {
        Core::Texture texture;
        const Header header                  = ReadFromFile<Header>(a_Stream);
        constexpr uint8_t FileIdentifier[12] = {
            0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
        };
        assert(memcmp(header.identifier, FileIdentifier, 12) == 0);
        assert(header.endianness == 0x04030201 && "file endianness does not match machine's");
        std::vector<KeyAndValue> keyAndValues;
        while (a_Stream.tellg() < sizeof(Header) + header.bytesOfKeyValueData) {
            auto keyAndValueByteSize = ReadFromFile<uint32_t>(a_Stream);
            auto keyAndValue         = ReadVectorFromFile<uint8_t>(a_Stream, keyAndValueByteSize);
            auto valuePadding        = ReadVectorFromFile<uint8_t>(a_Stream, 3 - ((keyAndValueByteSize + 3) % 4));
            auto keyEnd              = std::find(keyAndValue.begin(), keyAndValue.end(), uint8_t(0));
            keyAndValues.emplace_back(
                std::string { keyAndValue.begin(), keyEnd },
                std::vector<uint8_t> { keyEnd + 1, keyAndValue.end() });
        }
        auto pixelFormat = Core::Pixel::SizedFormat(Core::Pixel::GetSizedFormatBits(
            GetPixelFormat(header.glFormat),
            GetPixelType(header.glType),
            GetPixelType(header.glType),
            GetPixelType(header.glType),
            GetPixelType(header.glType)));
        // try to infer texture type from specs (ugh...)
        Core::TextureType textureType = Core::TextureType::Unknown;
        Core::ImageType imageType     = Core::ImageType::Unknown;
        bool isArray                  = header.numberOfArrayElements > 0;
        if (header.pixelSize.z > 0) {
            assert(!isArray && "3D texture array not supported");
            textureType = Core::TextureType::Texture3D;
            imageType   = Core::ImageType::Image3D;
        } else if (header.pixelSize.y > 0) {
            textureType = isArray ? Core::TextureType::Texture2DArray : Core::TextureType::Texture2D;
            imageType   = Core::ImageType::Image2D;
        } else {
            textureType = isArray ? Core::TextureType::Texture1DArray : Core::TextureType::Texture1D;
            imageType   = Core::ImageType::Image1D;
        }
        if (header.numberOfFaces == 6) {
            textureType = isArray ? Core::TextureType::TextureCubemapArray : Core::TextureType::TextureCubemap;
            imageType   = Core::ImageType::Cubemap;
        }
        auto mips     = std::max(header.numberOfMipmapLevels, 1u);
        auto elems    = std::max(header.numberOfArrayElements, 1u);
        auto faces    = std::max(header.numberOfFaces, 1u);
        auto baseSize = glm::max(header.pixelSize, 1u);
        for (auto level = 0u; level < mips; level++) {
            auto imageSize = ReadFromFile<uint32_t>(a_Stream);
            auto levelSize = glm::max(baseSize / unsigned(pow(2, level)), 1u);
            for (auto arrayElement = 0u; arrayElement < elems; arrayElement++) {
                for (auto face = 0u; face < faces; face++) {
                    auto buffer     = std::make_shared<Core::Buffer>(ReadVectorFromFile<std::byte>(a_Stream, imageSize));
                    auto bufferView = std::make_shared<Core::BufferView>(buffer, 0, imageSize);
                    if (imageType == Core::ImageType::Cubemap)
                        texture.emplace_back(std::make_shared<Core::Cubemap>(pixelFormat, levelSize.x, levelSize.y, bufferView));
                    else if (imageType == Core::ImageType::Image3D)
                        texture.emplace_back(std::make_shared<Core::Image3D>(pixelFormat, levelSize.x, levelSize.y, levelSize.z, bufferView));
                    else if (imageType == Core::ImageType::Image2D)
                        texture.emplace_back(std::make_shared<Core::Image2D>(pixelFormat, levelSize.x, levelSize.y, bufferView));
                    else if (imageType == Core::ImageType::Image1D)
                        texture.emplace_back(std::make_shared<Core::Image1D>(pixelFormat, levelSize.x, bufferView));
                    // cubePadding should be empty
                }
            }
            // mipPadding should be empty
        }
        texture.SetType(textureType);
        texture.SetPixelDescription(pixelFormat);
        texture.SetSize(baseSize);
        texture.SetCompressed(header.glType == 0 || header.glFormat == 0);
        a_Container->AddObject(std::make_shared<Core::Texture>(texture));
        return a_Container;
    }

    std::shared_ptr<Asset> ParseFromFile(const std::shared_ptr<Asset>& a_Container)
    {
        std::ifstream stream(a_Container->GetUri().DecodePath(), std::ios_base::binary);
        return ParseFromStream(a_Container, stream);
    }

    std::shared_ptr<Asset> ParseFromBinary(const std::shared_ptr<Asset>& a_Container)
    {
        auto binary = DataUri(a_Container->GetUri()).Decode();
        auto stream = std::istrstream(reinterpret_cast<const char*>(binary.data()), binary.size());
        return ParseFromStream(a_Container, stream);
    }
}

std::shared_ptr<Asset> ParseKTX(const std::shared_ptr<Asset>& a_Container)
{
    auto& uri = a_Container->GetUri();
    if (uri.GetScheme() == "data")
        return KTX::ParseFromBinary(a_Container);
    else
        return KTX::ParseFromFile(a_Container);
    return a_Container;
}

}
