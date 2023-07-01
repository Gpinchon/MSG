#include <Assets/Asset.hpp>
#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fstream>
#include <strstream>
#include <memory>

namespace TabGraph::Assets {
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
    auto bytes = stbi_load_from_callbacks(&cb, &a_Stream, &width, &height, &comp, 0);
    auto buffer = std::make_shared<SG::Buffer>(std::vector<std::byte>((std::byte*)bytes, (std::byte*)bytes + (width * height * comp)));
    stbi_image_free(bytes);
    auto image = std::make_shared<SG::Image>();
    switch (comp) {
    case 1:
        image->SetPixelDescription(SG::Pixel::SizedFormat::Uint8_NormalizedR);
        break;
    case 2:
        image->SetPixelDescription(SG::Pixel::SizedFormat::Uint8_NormalizedRG);
        break;
    case 3:
        image->SetPixelDescription(SG::Pixel::SizedFormat::Uint8_NormalizedRGB);
        break;
    case 4:
        image->SetPixelDescription(SG::Pixel::SizedFormat::Uint8_NormalizedRGBA);
        break;
    }
    image->SetBufferView(std::make_shared<SG::BufferView>(buffer, 0, buffer->size()));
    image->SetSize({ width, height, 1 });
    image->SetType(SG::Image::Type::Image2D);
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
    auto binary = DataUri(a_Container->GetUri()).Decode();
    auto stream = std::istrstream(reinterpret_cast<const char*>(binary.data()), binary.size());
    return ParseSTBFromStream(a_Container, stream);
}

std::shared_ptr<Asset> ParseSTBImage(const std::shared_ptr<Asset>& a_Container)
{
    auto& uri = a_Container->GetUri();
    if (uri.GetScheme() == "data")
        return ParseSTBFromBinary(a_Container);
    else
        return ParseSTBFromFile(a_Container);
    return a_Container;
}

}
