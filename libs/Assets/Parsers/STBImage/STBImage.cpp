#include <Assets/Asset.hpp>
#include <Core/Buffer/Buffer.hpp>
#include <Core/Buffer/View.hpp>
#include <Core/Image/Image2D.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <glm/common.hpp>
#include <stb_image.h>

#include <fstream>
#include <memory>
#include <strstream>

namespace MSG::Assets {
std::shared_ptr<Asset> ParseSTBFromStream(const std::shared_ptr<Asset>& a_Container, std::istream& a_Stream)
{
    int width = 0, height = 0, comp = 0;
    stbi_io_callbacks cb {};
    cb.read = [](void* a_User, char* a_Data, int a_Size) -> int {
        ((std::istream*)a_User)->read(a_Data, a_Size);
        return ((std::istream*)a_User)->gcount();
    };
    cb.skip = [](void* a_User, int a_N) {
        ((std::istream*)a_User)->ignore(a_N);
    };
    cb.eof = [](void* a_User) -> int {
        return ((std::istream*)a_User)->peek() == EOF;
    };
    stbi_info_from_callbacks(&cb, &a_Stream, &width, &height, &comp);
    a_Stream.clear();
    a_Stream.seekg(0);
    int compNbr = comp;
    if (comp == 1) // grey
        compNbr = 3;
    else if (comp == 2) // grey, alpha
        compNbr = 4;
    auto bytes      = stbi_load_from_callbacks(&cb, &a_Stream, &width, &height, &comp, compNbr);
    auto bufferSize = (width * height * compNbr);
    auto buffer     = std::make_shared<Core::Buffer>(std::vector<std::byte>((std::byte*)bytes, (std::byte*)bytes + bufferSize));
    stbi_image_free(bytes);
    Core::Pixel::SizedFormat pixelFormat = Core::Pixel::SizedFormat::Unknown;
    switch (compNbr) {
    case 3:
        pixelFormat = Core::Pixel::SizedFormat::Uint8_NormalizedRGB;
        break;
    case 4:
        pixelFormat = Core::Pixel::SizedFormat::Uint8_NormalizedRGBA;
        break;
    default:
        throw std::runtime_error("STBI parser : incorrect component nbr");
    }
    auto image           = std::make_shared<Core::Image2D>(pixelFormat, width, height, std::make_shared<Core::BufferView>(buffer, 0, buffer->size()));
    glm::uvec2 imageSize = image->GetSize();
    glm::uvec2 maxSize   = {
        a_Container->parsingOptions.image.maxWidth,
        a_Container->parsingOptions.image.maxHeight
    };
    if (glm::any(glm::greaterThan(imageSize, maxSize))) {
        auto newImageSize = glm::min(imageSize, maxSize);
        auto newImage     = std::make_shared<Core::Image2D>(image->GetPixelDescription(), newImageSize.x, newImageSize.y);
        newImage->Allocate();
        image->Blit(*newImage, { 0u, 0u, 0u }, image->GetSize(), Core::ImageFilter::Bilinear);
        image = newImage;
    }
    a_Container->AddObject(image);
    a_Container->SetLoaded(true);
    return a_Container;
}

std::shared_ptr<Asset> ParseSTBFromFile(const std::shared_ptr<Asset>& a_Container)
{
    std::ifstream stream(a_Container->GetUri().DecodePath(), std::ios_base::binary);
    return ParseSTBFromStream(a_Container, stream);
}

std::shared_ptr<Asset> ParseSTBFromBinary(const std::shared_ptr<Asset>& a_Container)
{
    std::vector<std::byte> binary;
    if (a_Container->parsingOptions.data.useBufferView) {
        Core::TypedBufferAccessor<std::byte> accessor(a_Container->GetBufferView(), 0, a_Container->GetBufferView()->GetByteLength());
        binary = { accessor.begin(), accessor.end() };
    } else
        binary = DataUri(a_Container->GetUri()).Decode();
    auto stream = std::istrstream(reinterpret_cast<const char*>(binary.data()), binary.size());
    return ParseSTBFromStream(a_Container, stream);
}

std::shared_ptr<Asset> ParseSTBImage(const std::shared_ptr<Asset>& a_Container)
{
    auto& uri = a_Container->GetUri();

    if (uri.GetScheme() == "data") {
        return ParseSTBFromBinary(a_Container);
    } else
        return ParseSTBFromFile(a_Container);
    return a_Container;
}

}
