#include <MSG/PageFile.hpp>
#include <cassert>
#include <ranges>

static constexpr MSG::PageCount RoundByteSize(const size_t& a_ByteSize)
{
    int remainder = a_ByteSize % MSG::PageSize;
    if (remainder == 0)
        return a_ByteSize;
    return a_ByteSize + MSG::PageSize - remainder;
}

MSG::PageFile& MSG::PageFile::Global()
{
    static PageFile s_PageFile;
    return s_PageFile;
}

MSG::PageFile::PageFile()
    : _pageFilePath(tmpnam(nullptr))
    , _pageFile(_pageFilePath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc)
{
}

MSG::PageFile::~PageFile()
{
    _pageFile.close();
    std::filesystem::remove(_pageFilePath);
}

MSG::PageID MSG::PageFile::Allocate(const size_t& a_ByteSize)
{
    const std::lock_guard lock(_mtx);
    auto requiredPageCount = RoundByteSize(a_ByteSize) / PageSize;
    if (_freePages.size() < requiredPageCount)
        _Resize(_pages.size() + requiredPageCount);
    PageID firstPageID = _freePages.front();
    for (size_t allocatedBytes = 0; allocatedBytes < a_ByteSize;) {
        PageID pageID = std::move(_freePages.front());
        _freePages.pop_front();
        auto& page = _pages.at(pageID);
        page.used  = std::min(a_ByteSize - allocatedBytes, PageSize);
        allocatedBytes += page.used;
        if (allocatedBytes < a_ByteSize && page.used == PageSize) // did we fill this page ?
            page.next = _freePages.front();
    }
    return firstPageID;
}

void MSG::PageFile::Release(const PageID& a_PageID)
{
    const std::lock_guard lock(_mtx);
    for (PageID id = a_PageID; id != NoPageID;) {
        _mappedRanges.erase(id);
        _freePages.push_back(id);
        auto& page = _pages.at(id);
        id         = page.next;
        page.used  = 0; // reset current page
        page.next  = NoPageID;
    }
}

std::vector<std::byte>& MSG::PageFile::Map(const PageID& a_PageID, const size_t& a_ByteOffset, const size_t& a_ByteSize)
{
    const std::lock_guard lock(_mtx);
    assert(!_mappedRanges.contains(a_PageID) && "Page already mapped");
    auto& mappedRange  = _mappedRanges[a_PageID];
    mappedRange.offset = a_ByteOffset;
    mappedRange.data   = Read(a_PageID, a_ByteOffset, a_ByteSize);
    return mappedRange.data;
}

void MSG::PageFile::Unmap(const PageID& a_PageID)
{
    const std::lock_guard lock(_mtx);
    auto itr          = _mappedRanges.find(a_PageID);
    auto& mappedRange = itr->second;
    Write(a_PageID, mappedRange.offset, std::move(mappedRange.data));
    _mappedRanges.erase(itr);
}

std::vector<std::byte> MSG::PageFile::Read(const PageID& a_PageID, const size_t& a_ByteOffset, const size_t& a_ByteSize)
{
    const std::lock_guard lock(_mtx);
    auto pages = _GetPages(a_PageID, a_ByteOffset, a_ByteSize);
    std::vector<std::byte> buffer(a_ByteSize);
    auto bufferItr = buffer.begin();
    for (auto& page : pages) {
        _pageFile.seekp(page.id * PageSize + page.offset);
        _pageFile.read(std::to_address(bufferItr), page.size);
        bufferItr += page.size;
    }
    assert(bufferItr == buffer.end() && "Couldn't read the required nbr of bytes");
    return std::move(buffer);
}

void MSG::PageFile::Write(const PageID& a_PageID, const size_t& a_ByteOffset, std::vector<std::byte>&& a_Data)
{
    const std::lock_guard lock(_mtx);
    auto pages     = _GetPages(a_PageID, a_ByteOffset, a_Data.size());
    auto bufferItr = a_Data.begin();
    for (auto& page : pages) {
        _pageFile.seekp(page.id * PageSize + page.offset);
        _pageFile.write(std::to_address(bufferItr), page.size);
        bufferItr += page.size; // increment data index
        if (bufferItr == a_Data.end())
            break;
    }
}

void MSG::PageFile::Shrink()
{
    const std::lock_guard lock(_mtx);
    PageCount newSize = _pages.size();
    for (auto& page : std::ranges::reverse_view(_pages)) {
        if (page.used == 0)
            newSize--;
        else
            break;
    }
    _Resize(newSize);
}

std::vector<MSG::PageFile::Range> MSG::PageFile::_GetPages(const PageID& a_PageID, const size_t& a_ByteOffset, const size_t& a_ByteSize)
{
    std::vector<Range> pages; // create local copy of necessary pages
    pages.reserve(_pages.size());
    {
        PageID id     = a_PageID;
        size_t offset = 0;
        size_t size   = 0;
        while (id != NoPageID && size < a_ByteSize) {
            auto& page = _pages.at(id);
            if (offset + page.used >= a_ByteOffset) { // is the required offset inside this page?
                auto usedOffset = offset > a_ByteOffset ? 0 : a_ByteOffset - offset;
                auto usedSize   = std::min(size_t(page.used), a_ByteSize - size) - usedOffset;
                pages.emplace_back(
                    id,
                    usedOffset,
                    usedSize);
                size += usedSize;
            }
            id = page.next;
            offset += page.used;
        }
    }
    return std::move(pages);
}

void MSG::PageFile::_Resize(const PageCount& a_Size)
{
    int64_t sizeDiff = int64_t(a_Size) - int64_t(_pages.size());
    if (sizeDiff > 0) { // we're growing
        for (PageID id = _pages.size(); id < a_Size; id++)
            _freePages.push_back(id);
    } else if (sizeDiff < 0) { // we're shriking
        for (size_t index = _pages.size(); index > a_Size; index--) {
            const PageID id = index - 1;
            // if this crashes, the page is not free
            _freePages.erase(std::find(_freePages.begin(), _freePages.end(), id));
        }
    } else // no size change
        return;
    _pages.resize(a_Size);
    std::filesystem::resize_file(_pageFilePath, a_Size * PageSize);
}
