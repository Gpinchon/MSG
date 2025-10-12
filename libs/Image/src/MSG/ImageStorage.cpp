#include <MSG/Image.hpp>
#include <MSG/ImageStorage.hpp>

#include <MSG/PageFile.hpp>
#include <MSG/PageRef.hpp>

#include <glm/common.hpp>
#include <glm/vec2.hpp>
#include <glm/vector_relational.hpp>

#ifdef MSG_RAM_IMAGE_STORAGE
Msg::ImageStorage::ImageStorage(const size_t& a_ByteSize)
    : _data(std::make_shared<std::vector<std::byte>>(a_ByteSize))
{
}

Msg::ImageStorage::ImageStorage(const std::vector<std::byte>& a_Data)
    : _data(std::make_shared<std::vector<std::byte>>(a_Data))
{
}

Msg::ImageStorage::ImageStorage(const ImageStorage& a_Src, const uint32_t& a_LayerOffset)
    : _data(a_Src._data)
    , _layerOffset(a_Src._layerOffset + a_LayerOffset)
{
}

void Msg::ImageStorage::Allocate(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc)
{
    const auto byteSize = a_PixDesc.GetPixelBufferByteSize(a_ImageSize);
    _data               = std::make_shared<std::vector<std::byte>>(byteSize);
}

void Msg::ImageStorage::Release()
{
    _data.reset();
}

std::vector<std::byte> Msg::ImageStorage::Read(
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

void Msg::ImageStorage::Write(
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

void Msg::ImageStorage::Map(
    const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc,
    const glm::uvec3& a_Offset, const glm::uvec3& a_Size)
{
}

void Msg::ImageStorage::Unmap(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc)
{
}

glm::vec4 Msg::ImageStorage::Read(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord)
{
    auto layerByteOffset = a_PixDesc.GetPixelBufferByteSize({ a_ImageSize.x, a_ImageSize.y, _layerOffset });
    auto index           = layerByteOffset + a_PixDesc.GetPixelIndex(a_ImageSize, a_TexCoord);
    assert(_data->size() >= index + a_PixDesc.GetPixelSize() && "Texel index out of bounds !");
    return a_PixDesc.GetColorFromBytes(&_data->at(index));
}

void Msg::ImageStorage::Write(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord, const glm::vec4& a_Color)
{
    auto layerByteOffset = a_PixDesc.GetPixelBufferByteSize({ a_ImageSize.x, a_ImageSize.y, _layerOffset });
    auto index           = layerByteOffset + a_PixDesc.GetPixelIndex(a_ImageSize, a_TexCoord);
    assert(_data->size() >= index + a_PixDesc.GetPixelSize() && "Texel index out of bounds !");
    a_PixDesc.SetColorToBytes(&_data->at(index), a_Color);
}

#else
Msg::ImageStorage::ImageStorage(const size_t& a_ByteSize)
    : _pageRef(std::make_shared<PageRef>(PageFile::Global(), PageFile::Global().Allocate(a_ByteSize)))
{
}

Msg::ImageStorage::ImageStorage(const std::vector<std::byte>& a_Data)
    : ImageStorage(a_Data.size())
{
    PageFile::Global().Write(*_pageRef, 0, std::move(a_Data));
}

Msg::ImageStorage::ImageStorage(const ImageStorage& a_Src, const uint32_t& a_LayerOffset)
    : _pageRef(a_Src._pageRef)
    , _layerOffset(a_Src._layerOffset + a_LayerOffset)
{
}

void Msg::ImageStorage::Allocate(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc)
{
    const auto byteSize = a_PixDesc.GetPixelBufferByteSize(a_ImageSize);
    _pageRef            = std::make_shared<PageRef>(PageFile::Global(), PageFile::Global().Allocate(byteSize));
}

void Msg::ImageStorage::Release()
{
    _pageRef.reset();
}

void Msg::ImageStorage::Clear(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc)
{
    PageFile::Global().Write(*_pageRef, 0,
        std::vector<std::byte>(a_PixDesc.GetPixelBufferByteSize(a_ImageSize), std::byte(0)));
}

void Msg::ImageStorage::Map(
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

void Msg::ImageStorage::Unmap(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc)
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

std::vector<std::byte> Msg::ImageStorage::Read(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_Offset, const glm::uvec3& a_Size)
{
    auto layerByteOffset = a_PixDesc.GetPixelBufferByteSize({ a_ImageSize.x, a_ImageSize.y, _layerOffset });
    if (a_Offset == glm::uvec3(0) && a_Size == a_ImageSize) {
        return PageFile::Global().Read(*_pageRef, layerByteOffset, a_PixDesc.GetPixelBufferByteSize(a_Size));
    }
    auto start  = a_Offset;
    auto extent = a_Size;
    auto end    = start + extent;
    if (a_PixDesc.GetSizedFormat() == Msg::PixelSizedFormat::DXT5_RGBA) {
        std::vector<std::byte> result;
        constexpr size_t blockByteSize = 16;
        constexpr glm::uvec3 blockSize = { 4, 4, 1 };
        auto blockCount                = ((a_ImageSize + (blockSize - 1u)) / blockSize);
        auto blockStart                = start / blockSize;
        auto blockExtent               = glm::max(extent / blockSize, 1u);
        auto blockEnd                  = blockStart + blockExtent;
        auto blockLineSize             = blockExtent.x * blockByteSize;
        assert(glm::all(glm::lessThanEqual(blockEnd, blockCount)));
        assert(extent % blockSize == glm::uvec3(0u));
        result.reserve(blockExtent.x * blockExtent.y * blockExtent.z * blockByteSize);
        for (auto z = blockStart.z; z < blockEnd.z; z++) {
            for (auto y = blockStart.y; y < blockEnd.y; y++) {
                auto blockIndex   = (z * blockCount.x * blockCount.y) + (y * blockCount.x) + blockStart.x;
                auto blockLineBeg = layerByteOffset + (blockIndex * blockByteSize);
                auto imageData    = PageFile::Global().Read(*_pageRef, blockLineBeg, blockLineSize);
                result.insert(result.end(),
                    imageData.begin(),
                    imageData.end());
            }
        }
        return result;
    } else {
        assert(glm::all(glm::lessThanEqual(end, a_ImageSize)) && "Pixel range out of bounds");
        std::vector<std::byte> result;
        result.reserve(a_PixDesc.GetPixelBufferByteSize(extent));
        auto lineByteSize = a_PixDesc.GetPixelBufferByteSize({ extent.x, 1, 1 });
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
}

void Msg::ImageStorage::Write(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_Offset, const glm::uvec3& a_Size, std::vector<std::byte> a_Data)
{
    auto layerByteOffset = a_PixDesc.GetPixelBufferByteSize({ a_ImageSize.x, a_ImageSize.y, _layerOffset });
    if (a_Offset == glm::uvec3(0) && a_Size == a_ImageSize) {
        assert(a_Data.size() == a_PixDesc.GetPixelBufferByteSize(a_Size) && "Incorrect pixel buffer size");
        return PageFile::Global().Write(*_pageRef, layerByteOffset, a_Data);
    }
    auto start  = a_Offset;
    auto extent = a_Size;
    std::lock_guard lock(PageFile::Global().GetLock());
    if (a_PixDesc.GetSizedFormat() == Msg::PixelSizedFormat::DXT5_RGBA) {
        constexpr size_t blockByteSize = 16;
        constexpr glm::uvec3 blockSize = { 4, 4, 1 };
        assert(a_Data.size() % blockByteSize == 0);
        const auto blockCount   = ((extent + (blockSize - 1u)) / blockSize);
        const auto lineByteSize = blockCount.x * blockByteSize;
        auto bufLineBeg         = a_Data.begin();
        for (auto z = 0u; z < extent.z; z += blockSize.z) {
            for (auto y = 0u; y < extent.y; y += blockSize.y) {
                const auto lineByteOffset = layerByteOffset + a_PixDesc.GetPixelIndex(a_ImageSize, start + glm::uvec3(0u, y, z));
                PageFile::Global().Write(*_pageRef, lineByteOffset, { bufLineBeg, bufLineBeg += lineByteSize });
            }
        }
    } else {
        assert(glm::all(glm::lessThanEqual(start + extent, a_ImageSize)) && "Pixel range out of bounds");
        auto bufLineBeg         = a_Data.begin();
        const auto lineByteSize = a_PixDesc.GetPixelBufferByteSize({ extent.x, 1, 1 });
        for (auto z = 0u; z < extent.z; z++) {
            for (auto y = 0u; y < extent.y; y++) {
                auto lineBeg = layerByteOffset + a_PixDesc.GetPixelIndex(a_ImageSize, start + glm::uvec3 { 0u, y, z });
                PageFile::Global().Write(*_pageRef, lineBeg, { bufLineBeg, bufLineBeg += lineByteSize });
            }
        }
    }
}

glm::vec4 Msg::ImageStorage::Read(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord)
{
    auto index = a_PixDesc.GetPixelIndex(_mappedSize, a_TexCoord - _mappedOffset);
    assert(_mappedBytes.size() >= index + a_PixDesc.GetPixelSize() && "Texture coordinates out of mapped bounds");
    if (a_PixDesc.GetSizedFormat() == PixelSizedFormat::DXT5_RGBA) {
        auto blockPtr  = std::to_address(_mappedBytes.begin() + index);
        auto blockSize = glm::uvec3 { 4, 4, 1 };
        auto colcoords = a_TexCoord % blockSize;
        auto colIndex  = static_cast<size_t>((colcoords.z * blockSize.x * blockSize.y) + (colcoords.y * blockSize.x) + colcoords.x);
        return a_PixDesc.DecompressBlock(blockPtr)[colIndex];
    } else
        return a_PixDesc.GetColorFromBytes(&_mappedBytes.at(index));
}

void Msg::ImageStorage::Write(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord, const glm::vec4& a_Color)
{
    auto index = a_PixDesc.GetPixelIndex(_mappedSize, a_TexCoord - _mappedOffset);
    assert(_mappedBytes.size() >= index + a_PixDesc.GetPixelSize() && "Texture coordinates out of mapped bounds");
    _modifiedBeg = glm::min(a_TexCoord, _modifiedBeg);
    _modifiedEnd = glm::max(a_TexCoord + 1u, _modifiedEnd);
    if (a_PixDesc.GetSizedFormat() == PixelSizedFormat::DXT5_RGBA) {
        auto blockPtr    = std::to_address(_mappedBytes.begin() + index);
        auto blockSize   = glm::uvec3 { 4, 4, 1 };
        auto colcoords   = a_TexCoord % blockSize;
        auto colIndex    = static_cast<size_t>((colcoords.z * blockSize.x * blockSize.y) + (colcoords.y * blockSize.x) + colcoords.x);
        auto colors      = a_PixDesc.DecompressBlock(blockPtr);
        colors[colIndex] = a_Color;
        std::memcpy(blockPtr, a_PixDesc.CompressBlock(colors.data()).data(), 16);
    } else
        a_PixDesc.SetColorToBytes(&_mappedBytes.at(index), a_Color);
}
#endif