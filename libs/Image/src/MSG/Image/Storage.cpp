#include <MSG/Image/Storage.hpp>

#include "Storage.hpp"
#include <MSG/Image.hpp>
#include <MSG/PageFile.hpp>

static inline size_t GetBufferIndex(const glm::uvec3& a_ImageSize, const MSG::PixelDescriptor& a_PixDesc, const glm::uvec3& a_PixCoord)
{
    return static_cast<size_t>((a_PixCoord.z * a_ImageSize.x * a_ImageSize.y) + (a_PixCoord.y * a_ImageSize.x) + a_PixCoord.x);
}

MSG::ImageStorage::ImageStorage(const ImageInfo& a_Info)
{
    const auto pixelCount = a_Info.width * a_Info.height * a_Info.depth;
    const auto byteSize   = a_Info.pixelDesc.GetPixelSize() * pixelCount;
    _pageID               = PageFile::Global().Allocate(byteSize);
}

MSG::ImageStorage::~ImageStorage()
{
    PageFile::Global().Release(_pageID);
}

void MSG::ImageStorage::Map(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_Offset, const glm::uvec3& a_Size)
{
    auto begCoord     = a_Offset;
    auto endCoord     = begCoord + a_Size;
    auto begIndex     = a_PixDesc.GetPixelIndex(a_ImageSize, begCoord);
    auto endIndex     = a_PixDesc.GetPixelIndex(a_ImageSize, endCoord);
    _mappedByteOffset = begIndex;
    _mappedBytes      = &PageFile::Global().Map(_pageID, begIndex, endIndex);
}

void MSG::ImageStorage::Unmap()
{
    PageFile::Global().Unmap(_pageID);
}

glm::vec4 MSG::ImageStorage::Read(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord)
{
    auto index = a_PixDesc.GetPixelIndex(a_ImageSize, a_TexCoord) - _mappedByteOffset;
    return a_PixDesc.GetColorFromBytes(&_mappedBytes->at(index));
}

void MSG::ImageStorage::Write(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord, const glm::vec4& a_Color)
{
    auto index = a_PixDesc.GetPixelIndex(a_ImageSize, a_TexCoord) - _mappedByteOffset;
    a_PixDesc.SetColorToBytes(&_mappedBytes->at(index), a_Color);
}