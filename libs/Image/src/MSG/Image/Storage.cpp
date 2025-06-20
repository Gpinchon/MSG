#include <MSG/Image/Storage.hpp>

#include <MSG/Image.hpp>
#include <MSG/PageFile.hpp>
#include <MSG/PageRef.hpp>

#include <glm/common.hpp>
#include <glm/vec2.hpp>
#include <glm/vector_relational.hpp>

static inline size_t GetBufferIndex(const glm::uvec3& a_ImageSize, const MSG::PixelDescriptor& a_PixDesc, const glm::uvec3& a_PixCoord)
{
    return static_cast<size_t>((a_PixCoord.z * a_ImageSize.x * a_ImageSize.y) + (a_PixCoord.y * a_ImageSize.x) + a_PixCoord.x);
}

MSG::ImageStorage::ImageStorage(const size_t& a_ByteSize)
    : _pageRef(std::make_shared<PageRef>(PageFile::Global(), PageFile::Global().Allocate(a_ByteSize)))
{
}

MSG::ImageStorage::ImageStorage(std::vector<std::byte>&& a_Data)
    : ImageStorage(a_Data.size())
{
    PageFile::Global().Write(*_pageRef, 0, std::move(a_Data));
}

MSG::ImageStorage::ImageStorage(const std::shared_ptr<PageRef>& a_PageRef, const glm::uvec3& a_Offset)
    : _pageRef(a_PageRef)
    , _pageOffset(a_Offset)
{
}

MSG::ImageStorage::ImageStorage(const ImageStorage& a_Src, const glm::uvec3& a_Offset)
    : ImageStorage(a_Src._pageRef, a_Src._pageOffset + a_Offset)
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

std::vector<std::byte>& MSG::ImageStorage::Map(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_Offset, const glm::uvec3& a_Size)
{
    assert(_mappedOffset == glm::uvec3(-1u) && "Storage already mapped!");
    assert(_mappedSize == glm::uvec3(0) && "Storage already mapped!");
    assert(_mappedBytes.empty() && "Storage already mapped!");
    _mappedOffset = a_Offset;
    _mappedSize   = a_Size;
    _mappedBytes  = Read(a_ImageSize, a_PixDesc, a_Offset, a_Size);
    _modifiedBeg  = glm::uvec3(-1u);
    _modifiedEnd  = glm::uvec3(0u);
    return _mappedBytes;
}

void MSG::ImageStorage::Unmap(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc)
{
    if (_modifiedBeg != glm::uvec3(-1u)) {
        const auto modifiedOffset = a_PixDesc.GetPixelIndex(_mappedSize, _modifiedBeg - _mappedOffset);
        const auto modifiedRange  = _modifiedEnd - _modifiedBeg;
        std::vector<std::byte> data(
            std::make_move_iterator(_mappedBytes.begin() + modifiedOffset),
            std::make_move_iterator(_mappedBytes.end()));
        Write(a_ImageSize, a_PixDesc, _modifiedBeg, modifiedRange + 1u, std::move(data));
    }
    _mappedBytes.clear();
    _mappedBytes.shrink_to_fit();
    _mappedBytes.reserve(PageSize);
    _mappedOffset = glm::uvec3(-1u);
    _mappedSize   = glm::uvec3(0);
}

std::vector<std::byte> MSG::ImageStorage::Read(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_Offset, const glm::uvec3& a_Size)
{
    if (glm::uvec2(_pageOffset) == glm::uvec2(0) && a_Offset == glm::uvec3 { 0u, 0u, 0u } && a_Size == a_ImageSize) {
        auto layerOffset = _pageOffset.z * a_PixDesc.GetPixelBufferByteSize({ a_Size.x, a_Size.y, 1 });
        return PageFile::Global().Read(*_pageRef, layerOffset, a_PixDesc.GetPixelBufferByteSize(a_Size));
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
            auto lineBeg   = a_PixDesc.GetPixelIndex(a_ImageSize, _pageOffset + glm::uvec3 { start.x, y, z });
            auto imageData = PageFile::Global().Read(*_pageRef, lineBeg, lineByteSize);
            result.insert(result.end(),
                std::make_move_iterator(imageData.begin()),
                std::make_move_iterator(imageData.end()));
        }
    }
    return result;
}

void MSG::ImageStorage::Write(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_Offset, const glm::uvec3& a_Size, std::vector<std::byte> a_Data)
{
    if (glm::uvec2(_pageOffset) == glm::uvec2(0) && a_Offset == glm::uvec3(0) && a_Size == a_ImageSize) {
        assert(a_Data.size() == a_PixDesc.GetPixelBufferByteSize(a_Size) && "Incorrect pixel buffer size");
        auto layerOffset = _pageOffset.z * a_PixDesc.GetPixelBufferByteSize({ a_ImageSize.x, a_ImageSize.y, 1 });
        return PageFile::Global().Write(*_pageRef, layerOffset, a_Data);
    }
    assert(glm::all(glm::lessThanEqual(a_Offset + a_Size, a_ImageSize)) && "Pixel range out of bounds");
    std::lock_guard lock(PageFile::Global().GetLock());
    auto bufLineBeg        = a_Data.begin();
    const auto bufLineSize = _mappedSize.x * a_PixDesc.GetPixelSize();
    for (auto z = 0u; z < a_Size.z; z++) {
        for (auto y = 0u; y < a_Size.y; y++) {
            auto lineBeg          = a_PixDesc.GetPixelIndex(a_ImageSize, _pageOffset + a_Offset + glm::uvec3 { 0u, y, z });
            const auto bufLineEnd = bufLineBeg + bufLineSize;
            PageFile::Global().Write(*_pageRef, lineBeg, { bufLineBeg, bufLineEnd });
            bufLineBeg = bufLineEnd;
        }
    }
}

glm::vec4 MSG::ImageStorage::Read(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord)
{
    auto index = a_PixDesc.GetPixelIndex(_mappedSize, a_TexCoord - _mappedOffset);
    assert(glm::all(glm::greaterThanEqual(a_TexCoord, _mappedOffset)) && "Texture coordinates out of mapped bounds");
    assert(glm::all(glm::lessThan(a_TexCoord, _mappedOffset + _mappedSize)) && "Texture coordinates out of mapped bounds");
    assert(_mappedBytes.size() >= index + a_PixDesc.GetPixelSize());
    return a_PixDesc.GetColorFromBytes(&_mappedBytes.at(index));
}

void MSG::ImageStorage::Write(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord, const glm::vec4& a_Color)
{
    auto index = a_PixDesc.GetPixelIndex(_mappedSize, a_TexCoord - _mappedOffset);
    assert(glm::all(glm::greaterThanEqual(a_TexCoord, _mappedOffset)) && "Texture coordinates out of mapped bounds");
    assert(glm::all(glm::lessThan(a_TexCoord, _mappedOffset + _mappedSize)) && "Texture coordinates out of mapped bounds");
    assert(_mappedBytes.size() >= index + a_PixDesc.GetPixelSize());
    _modifiedBeg = glm::min(a_TexCoord, _modifiedBeg);
    _modifiedEnd = glm::max(a_TexCoord, _modifiedEnd);
    a_PixDesc.SetColorToBytes(&_mappedBytes.at(index), a_Color);
}