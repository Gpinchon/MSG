#include <MSG/Assets/Asset.hpp>
#include <MSG/Debug.hpp>
#include <MSG/Image.hpp>
#include <MSG/ImageUtils.hpp>
#include <MSG/Texture.hpp>

#include <DDSHeader.hpp>

#include <bitset>
#include <fstream>
#include <strstream>

namespace Msg::Assets {
#define READ_DATA(stream, dest, size)                            \
    {                                                            \
        stream.read((char*)&dest, size);                         \
        MSGCheckErrorFatal(!stream, "Error while reading file"); \
    }

static std::string FourCCToString(uint8_t* a)
{
    int i;
    std::string s = "";
    for (i = 0; i < 4; i++) {
        s = s + (char)a[i];
    }
    return s;
}

static std::shared_ptr<Asset> ParseDDSFromStream(const std::shared_ptr<Asset>& a_Container, std::istream& a_Stream)
{
    Msg::Texture texture;
    DDSMagicWord magicWord = 0;
    DDSHeader header       = {};
    READ_DATA(a_Stream, magicWord, sizeof(magicWord));
    MSGCheckErrorFatal(magicWord != 0x20534444, "DDS magic word is wrong");
    READ_DATA(a_Stream, header, sizeof(header));
    if ((header.pixelFormat.flags & DDSPixelFlag::FourCC) != 0) {
        std::string fourCC = FourCCToString(header.pixelFormat.fourCC);
        if (fourCC == "DX10") {
            DDSHeaderDX10 headerDX10 = {};
            READ_DATA(a_Stream, headerDX10, sizeof(headerDX10));
        } else if (fourCC == "DXT5") {
            texture.SetPixelDescriptor(PixelSizedFormat::DXT5_RGBA);
        } else
            MSGErrorFatal("DDS Pixel Format is not managed : " + fourCC);
    }
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
