#include <MSG/PageFile.hpp>

#include <algorithm>
#include <cassert>
#include <ranges>
#include <string>

#ifdef _WIN32
#include <io.h>
#define ftruncate  _chsize_s
#define ftell      _ftelli64_nolock
#define fseek      _fseeki64_nolock
#define fwrite     _fwrite_nolock
#define fread      _fread_nolock
#define fflush     _fflush_nolock
#define filelength _filelengthi64
#else
#include <unistd.h>
#endif

MSG::PageCount MSG::PageFile::RoundByteSize(const size_t& a_ByteSize)
{
    size_t remainder = a_ByteSize % MSG::PageSize;
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
    : _pageFile(std::tmpfile())
{
}

MSG::PageFile::~PageFile()
{
    std::fclose(_pageFile);
}

MSG::PageID MSG::PageFile::Allocate(const size_t& a_ByteSize)
{
    const std::lock_guard lock(_mtx);
    size_t requiredPageCount = RoundByteSize(a_ByteSize) / PageSize;
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
        _freePages.push_back(id);
        auto& page = _pages.at(id);
        id         = page.next;
        page       = {}; // reset current page
    }
    // shrink the container if we have at least twice the number of free pages compared to the total number of pages
    if (_freePages.size() >= _pages.size() / 2)
        Shrink();
}

static inline size_t RoundUpToPage(const size_t& a_ByteSize)
{
    return (1 + (a_ByteSize - 1) / MSG::PageSize) * MSG::PageSize;
}

std::vector<std::byte> MSG::PageFile::Read(const PageID& a_PageID, const size_t& a_ByteOffset, const size_t& a_ByteSize)
{
    const std::lock_guard lock(_mtx);
    auto currentPageID = a_PageID;
    for (size_t pagesToSkip = a_ByteOffset / PageSize; pagesToSkip > 0; pagesToSkip--) {
        currentPageID = _pages.at(currentPageID).next;
        assert(currentPageID != NoPageID && "Byte offset out of bounds");
    }
    size_t remainingOffset = a_ByteOffset - (a_ByteOffset / PageSize) * PageSize;
    std::vector<std::byte> pages(RoundUpToPage(remainingOffset + a_ByteSize));
    for (auto pagesItr = pages.begin(); pagesItr != pages.end();) {
        fseek(_pageFile, currentPageID * PageSize, SEEK_SET);
        pagesItr += fread(std::to_address(pagesItr), 1, PageSize, _pageFile);
        assert(currentPageID != NoPageID && "Byte size out of bounds");
        currentPageID = _pages.at(currentPageID).next;
    }
    const auto pagesItr = pages.begin() + remainingOffset;
    return { pagesItr, pagesItr + a_ByteSize };
}

void MSG::PageFile::Write(const PageID& a_PageID, const size_t& a_ByteOffset, std::vector<std::byte> a_Data)
{
    const std::lock_guard lock(_mtx);
    auto currentPageID = a_PageID;
    for (size_t pagesToSkip = a_ByteOffset / PageSize; pagesToSkip > 0; pagesToSkip--) {
        currentPageID = _pages.at(currentPageID).next;
        assert(currentPageID != NoPageID && "Byte offset out of bounds");
    }
    size_t remainingOffset = a_ByteOffset - (a_ByteOffset / PageSize) * PageSize;
    for (auto bufferItr = a_Data.begin(); bufferItr != a_Data.end();) {
        assert(currentPageID != NoPageID && "Byte size out of bounds");
        size_t writeSize = std::min(PageSize, size_t(std::distance(bufferItr, a_Data.end())));
        writeSize        = remainingOffset < writeSize ? writeSize - remainingOffset : writeSize;
        fseek(_pageFile, currentPageID * PageSize + remainingOffset, SEEK_SET) == 0;
        bufferItr += fwrite(std::to_address(bufferItr), 1, writeSize, _pageFile);
        currentPageID   = _pages.at(currentPageID).next;
        remainingOffset = 0;
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

void MSG::PageFile::_Resize(const PageCount& a_Size)
{
    if (a_Size > _pages.size()) { // we're growing
        for (PageID id = _pages.size(); id < a_Size; id++)
            _freePages.push_back(id);
    } else if (a_Size < _pages.size()) { // we're shriking
        for (size_t index = _pages.size(); index > a_Size; index--) {
            const PageID id = index - 1;
            // if this crashes, the page is not free
            _freePages.erase(std::find(_freePages.begin(), _freePages.end(), id));
        }
    } else // no size change
        return;
    _pages.resize(a_Size);
    fflush(_pageFile);
    const size_t newByteSize = a_Size * PageSize;
    if (ftruncate(fileno(_pageFile), newByteSize) != 0)
        throw std::runtime_error("Could not resize page file to new size: " + std::to_string(newByteSize));
#ifndef _NDEBUG
    const size_t newFileSize = filelength(fileno(_pageFile));
    assert(newFileSize == newByteSize && "Page file resizing failed !");
#endif //_NDEBUG
}
