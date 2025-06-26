#include <MSG/Image/Storage.hpp>

#include <MSG/Image.hpp>
#include <MSG/PageFile.hpp>
#include <MSG/PageRef.hpp>

#include <glm/common.hpp>
#include <glm/vec2.hpp>
#include <glm/vector_relational.hpp>

#ifdef MSG_RAM_IMAGE_STORAGE
MSG::ImageStorage::ImageStorage(const size_t& a_ByteSize)
    : _data(std::make_shared<std::vector<std::byte>>(a_ByteSize))
{
}

MSG::ImageStorage::ImageStorage(const std::vector<std::byte>& a_Data)
    : _data(std::make_shared<std::vector<std::byte>>(a_Data))
{
}

MSG::ImageStorage::ImageStorage(const ImageStorage& a_Src, const uint32_t& a_LayerOffset)
    : _data(a_Src._data)
    , _layerOffset(a_Src._layerOffset + a_LayerOffset)
{
}

void MSG::ImageStorage::Allocate(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc)
{
    const auto byteSize = a_PixDesc.GetPixelBufferByteSize(a_ImageSize);
    _data               = std::make_shared<std::vector<std::byte>>(byteSize);
}

void MSG::ImageStorage::Release()
{
    _data.reset();
}

std::vector<std::byte> MSG::ImageStorage::Read(
    const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc,
    const glm::uvec3& a_Offset, const glm::uvec3& a_Size)
{
    auto layerByteOffset = a_PixDesc.GetPixelBufferByteSize({ a_ImageSize.x, a_ImageSize.y, _layerOffset });
    auto start           = a_Offset;
    auto extent          = a_Size;
    auto end             = start + extent;
    assert(glm::all(glm::lessThanEqual(end, a_ImageSize)) && "Pixel range out of bounds");
    std::vector<std::byte> result;
    result.reserve(a_PixDesc.GetPixelBufferByteSize(extent));
    auto lineByteSize = a_PixDesc.GetPixelBufferByteSize({ extent.x, 1, 1 }); // TODO make this work for compressed image!
    for (auto z = start.z; z < end.z; z++) {
        for (auto y = start.y; y < end.y; y++) {
            const auto lineBeg    = layerByteOffset + a_PixDesc.GetPixelIndex(a_ImageSize, glm::uvec3 { start.x, y, z });
            const auto lineBegItr = _data->begin() + lineBeg;
            const auto lineEndItr = lineBegItr + lineByteSize;
            result.insert(result.end(),
                std::make_move_iterator(lineBegItr),
                std::make_move_iterator(lineEndItr));
        }
    }
    return result;
}

void MSG::ImageStorage::Write(
    const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc,
    const glm::uvec3& a_Offset, const glm::uvec3& a_Size, std::vector<std::byte> a_Data)
{
    auto layerByteOffset = a_PixDesc.GetPixelBufferByteSize({ a_ImageSize.x, a_ImageSize.y, _layerOffset });
    auto start           = a_Offset;
    auto extent          = a_Size;
    auto end             = start + extent;
    assert(glm::all(glm::lessThanEqual(end, a_ImageSize)) && "Pixel range out of bounds");
    std::lock_guard lock(PageFile::Global().GetLock());
    auto bufLineBegItr      = a_Data.begin();
    const auto lineByteSize = a_PixDesc.GetPixelBufferByteSize({ extent.x, 1, 1 });
    for (auto z = 0u; z < a_Size.z; z++) {
        for (auto y = 0u; y < a_Size.y; y++) {
            const auto lineBeg = layerByteOffset + a_PixDesc.GetPixelIndex(a_ImageSize, a_Offset + glm::uvec3 { 0u, y, z });
            std::memcpy(std::to_address(_data->begin() + lineBeg), std::to_address(bufLineBegItr), lineByteSize);
            bufLineBegItr += lineByteSize;
        }
    }
}

void MSG::ImageStorage::Map(
    const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc,
    const glm::uvec3& a_Offset, const glm::uvec3& a_Size)
{
}

void MSG::ImageStorage::Unmap(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc)
{
}

glm::vec4 MSG::ImageStorage::Read(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord)
{
    auto layerByteOffset = a_PixDesc.GetPixelBufferByteSize({ a_ImageSize.x, a_ImageSize.y, _layerOffset });
    auto index           = layerByteOffset + a_PixDesc.GetPixelIndex(a_ImageSize, a_TexCoord);
    assert(_data->size() >= index + a_PixDesc.GetPixelSize() && "Texel index out of bounds !");
    return a_PixDesc.GetColorFromBytes(&_data->at(index));
}

void MSG::ImageStorage::Write(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord, const glm::vec4& a_Color)
{
    auto layerByteOffset = a_PixDesc.GetPixelBufferByteSize({ a_ImageSize.x, a_ImageSize.y, _layerOffset });
    auto index           = layerByteOffset + a_PixDesc.GetPixelIndex(a_ImageSize, a_TexCoord);
    assert(_data->size() >= index + a_PixDesc.GetPixelSize() && "Texel index out of bounds !");
    a_PixDesc.SetColorToBytes(&_data->at(index), a_Color);
}

#else
MSG::ImageStorage::ImageStorage(const size_t& a_ByteSize)
    : _pageRef(std::make_shared<PageRef>(PageFile::Global(), PageFile::Global().Allocate(a_ByteSize)))
{
}

MSG::ImageStorage::ImageStorage(const std::vector<std::byte>& a_Data)
    : ImageStorage(a_Data.size())
{
    PageFile::Global().Write(*_pageRef, 0, std::move(a_Data));
}

MSG::ImageStorage::ImageStorage(const ImageStorage& a_Src, const uint32_t& a_LayerOffset)
    : _pageRef(a_Src._pageRef)
    , _layerOffset(a_Src._layerOffset + a_LayerOffset)
{
}

void MSG::ImageStorage::Allocate(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc)
{
    const auto byteSize = a_PixDesc.GetPixelBufferByteSize(a_ImageSize);
    _pageRef            = std::make_shared<PageRef>(PageFile::Global(), PageFile::Global().Allocate(byteSize));
}

void MSG::ImageStorage::Release()
{
    _pageRef.reset();
}

void MSG::ImageStorage::Map(
    const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc,
    const glm::uvec3& a_Offset, const glm::uvec3& a_Size)
{
    assert(_mappedBytes.empty() && "Storage already mapped!");
    _mappedOffset = a_Offset;
    _mappedSize   = a_Size;
    _mappedBytes  = Read(a_ImageSize, a_PixDesc, _mappedOffset, _mappedSize);
    _modifiedBeg  = glm::uvec3(-1u);
    _modifiedEnd  = glm::uvec3(0u);
}

void MSG::ImageStorage::Unmap(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc)
{
    if (_modifiedBeg != glm::uvec3(-1u)) {
        Write(a_ImageSize, a_PixDesc, _mappedOffset, _mappedSize, _mappedBytes);
    }
    _mappedBytes.clear();
    _mappedBytes.shrink_to_fit();
    _mappedBytes.reserve(PageSize);
    _mappedOffset = glm::uvec3(-1u);
    _mappedSize   = glm::uvec3(0);
}

std::vector<std::byte> MSG::ImageStorage::Read(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_Offset, const glm::uvec3& a_Size)
{
    auto layerByteOffset = a_PixDesc.GetPixelBufferByteSize({ a_ImageSize.x, a_ImageSize.y, _layerOffset });
    if (a_Offset == glm::uvec3(0) && a_Size == a_ImageSize) {
        return PageFile::Global().Read(*_pageRef, layerByteOffset, a_PixDesc.GetPixelBufferByteSize(a_Size));
    }
    auto start  = a_Offset;
    auto extent = a_Size;
    auto end    = start + extent;
    assert(glm::all(glm::lessThanEqual(end, a_ImageSize)) && "Pixel range out of bounds");
    std::vector<std::byte> result;
    result.reserve(a_PixDesc.GetPixelBufferByteSize(extent));
    auto lineByteSize = a_PixDesc.GetPixelBufferByteSize({ extent.x, 1, 1 }); // TODO make this work for compressed image!
    std::lock_guard lock(PageFile::Global().GetLock());
    for (auto z = start.z; z < end.z; z++) {
        for (auto y = start.y; y < end.y; y++) {
            auto lineBeg   = layerByteOffset + a_PixDesc.GetPixelIndex(a_ImageSize, glm::uvec3 { start.x, y, z });
            auto imageData = PageFile::Global().Read(*_pageRef, lineBeg, lineByteSize);
            result.insert(result.end(),
                imageData.begin(),
                imageData.end());
        }
    }
    return result;
}

void MSG::ImageStorage::Write(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_Offset, const glm::uvec3& a_Size, std::vector<std::byte> a_Data)
{
    auto layerByteOffset = a_PixDesc.GetPixelBufferByteSize({ a_ImageSize.x, a_ImageSize.y, _layerOffset });
    if (a_Offset == glm::uvec3(0) && a_Size == a_ImageSize) {
        assert(a_Data.size() == a_PixDesc.GetPixelBufferByteSize(a_Size) && "Incorrect pixel buffer size");
        return PageFile::Global().Write(*_pageRef, layerByteOffset, a_Data);
    }
    auto start  = a_Offset;
    auto extent = a_Size;
    auto end    = start + extent;
    assert(glm::all(glm::lessThanEqual(end, a_ImageSize)) && "Pixel range out of bounds");
    std::lock_guard lock(PageFile::Global().GetLock());
    auto bufLineBeg        = a_Data.begin();
    const auto bufLineSize = _mappedSize.x * a_PixDesc.GetPixelSize();
    for (auto z = 0u; z < extent.z; z++) {
        for (auto y = 0u; y < extent.y; y++) {
            auto lineBeg          = layerByteOffset + a_PixDesc.GetPixelIndex(a_ImageSize, start + glm::uvec3 { 0u, y, z });
            const auto bufLineEnd = bufLineBeg + bufLineSize;
            PageFile::Global().Write(*_pageRef, lineBeg, { bufLineBeg, bufLineEnd });
            bufLineBeg = bufLineEnd;
        }
    }
}

glm::vec4 MSG::ImageStorage::Read(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord)
{
    auto index = a_PixDesc.GetPixelIndex(_mappedSize, a_TexCoord - _mappedOffset);
    assert(_mappedBytes.size() >= index + a_PixDesc.GetPixelSize() && "Texture coordinates out of mapped bounds");
    return a_PixDesc.GetColorFromBytes(&_mappedBytes.at(index));
}

void MSG::ImageStorage::Write(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord, const glm::vec4& a_Color)
{
    auto index = a_PixDesc.GetPixelIndex(_mappedSize, a_TexCoord - _mappedOffset);
    assert(_mappedBytes.size() >= index + a_PixDesc.GetPixelSize() && "Texture coordinates out of mapped bounds");
    _modifiedBeg = glm::min(a_TexCoord, _modifiedBeg);
    _modifiedEnd = glm::max(a_TexCoord + 1u, _modifiedEnd);
    a_PixDesc.SetColorToBytes(&_mappedBytes.at(index), a_Color);
}
#endif