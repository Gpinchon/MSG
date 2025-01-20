/*
 * @Author: gpinchon
 * @Date:   2019-02-22 16:13:28
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-01-11 08:46:17
 */

#include <Assets/Asset.hpp>
#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image2D.hpp>

#include <fstream>
#include <strstream>

namespace MSG::Assets {
namespace KTX {
template<typename T>
T ReadFromFile(std::istream& a_Stream) {
    T data {};
    a_Stream.read((char*)&data, sizeof(T));
    return data;
}

template<typename T>
std::vector<T> ReadVectorFromFile(std::istream& a_Stream, const size_t& a_Count) {
    std::vector<T> data(a_Count);
    a_Stream.read((char*)data.data(), sizeof(T) * a_Count);
    return data;
}

struct Header {
    char identifier[12];
    uint32_t vkFormat;
    uint32_t typeSize;
    uint32_t pixelWidth;
    uint32_t pixelHeight;
    uint32_t pixelDepth;
    uint32_t layerCount;
    uint32_t faceCount;
    uint32_t levelCount;
    uint32_t supercompressionScheme;
};

struct Index {
    uint32_t dfdByteOffset;
    uint32_t dfdByteLength;
    uint32_t kvdByteOffset;
    uint32_t kvdByteLength;
    uint64_t sgdByteOffset;
    uint64_t sgdByteLength;
};

struct LevelIndex {
    uint64_t byteOffset;
    uint64_t byteLength;
    uint64_t uncompressedByteLength;
};

struct DataFormatDescriptor {
    unsigned flags : 8;
    unsigned transferFunction : 8;
    unsigned colorPrimaries : 8;
    unsigned colorModel : 8;
    unsigned texelBlockDimension3 : 8;
    unsigned texelBlockDimension2 : 8;
    unsigned texelBlockDimension1 : 8;
    unsigned texelBlockDimension0 : 8;
    unsigned bytesPlane3 : 8;
    unsigned bytesPlane2 : 8;
    unsigned bytesPlane1 : 8;
    unsigned bytesPlane0 : 8;
    unsigned bytesPlane7 : 8;
    unsigned bytesPlane6 : 8;
    unsigned bytesPlane5 : 8;
    unsigned bytesPlane4 : 8;
};

struct DescriptorBlockHeader {
    unsigned descriptorType : 15;
    unsigned vendorId : 17;
    unsigned descriptorBlockSize : 16;
    unsigned versionNumber : 16;
    union {
        DataFormatDescriptor dataFormat;
    };
};

class File {
    File(std::istream& a_Stream) : stream(a_Stream) {};
    std::istream& stream;
    Header header = ReadFromFile<Header>(stream);
    Index index   = ReadFromFile<Index>(stream);
    std::vector<LevelIndex> levelIndex = ReadVectorFromFile<LevelIndex>(stream, std::max(1u, header.levelCount));
};

std::shared_ptr<Asset> ParseFromStream(const std::shared_ptr<Asset>& a_Container, std::istream& a_Stream) {
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
