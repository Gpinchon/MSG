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

MSG::PageFile::PageFile()
    : _libFilePath(tmpnam(nullptr))
    , _pageFilePath(tmpnam(nullptr))
    , _libFile(_libFilePath, std::ios_base::in | std::ios_base::out | std::ios_base::trunc)
    , _pageFile(_pageFilePath, std::ios_base::in | std::ios_base::out | std::ios_base::trunc)
{
}

MSG::PageFile::PageFile(std::FILE* a_LibFile, std::FILE* a_PageFile)
    : _libFile(a_LibFile)
    , _pageFile(a_PageFile)
{
}

MSG::PageFile::~PageFile()
{
    _libFile.close();
    _pageFile.close();
    std::filesystem::remove(_libFilePath);
    std::filesystem::remove(_pageFilePath);
}

MSG::PageID MSG::PageFile::Allocate(const size_t& a_ByteSize)
{
    auto newPageCount = _pages.size() + RoundByteSize(a_ByteSize) / PageSize;
    _Resize(newPageCount);
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
    for (PageID id = a_PageID; id != -1u;) {
        _freePages.push_back(id);
        auto& page = _pages.at(id);
        id         = page.next;
        page.used  = 0; // reset current page
        page.next  = -1u;
    }
}

std::vector<std::byte> MSG::PageFile::Read(const PageID& a_PageID, const size_t& a_ByteSize)
{
    std::vector<std::byte> buffer;
    buffer.reserve(a_ByteSize);
    _thread.PushSynchronousCommand(
        [this, id = a_PageID, targetSize = a_ByteSize, &buffer]() mutable {
            while (buffer.size() < targetSize) {
                auto& page = _pages.at(id);
                if (page.used == 0)
                    break; // check if current data is used
                auto readSize = std::min(size_t(page.used), targetSize - buffer.size());
                buffer.resize(buffer.size() + readSize);
                _pageFile.seekp(id * PageSize);
                _pageFile.read(std::to_address(buffer.end() - readSize), readSize);
                id = page.next;
            }
            assert(buffer.size() == targetSize && "Couldn't read the required nbr of bytes");
        });
    return std::move(buffer);
}

void MSG::PageFile::Write(const PageID& a_PageID, std::vector<std::byte>&& a_Data)
{
    std::vector<Page> pages; // create local copy of pages
    pages.reserve(_pages.size());
    for (PageID id = a_PageID; id != -1u;) {
        auto& page = _pages.at(id);
        pages.emplace_back(page);
        id = page.next;
    }
    _thread.PushCommand([this, id = a_PageID, pages = std::move(pages), data = std::move(a_Data)]() mutable {
        size_t pageIndex    = 0;
        size_t writtenBytes = 0;
        while (writtenBytes < data.size()) {
            auto& page = pages.at(pageIndex);
#ifndef _NDEBUG
            auto used = std::min(data.size() - writtenBytes, PageSize);
            assert(used <= page.used && "Page out of bound");
#endif
            _pageFile.seekp(id * PageSize);
            _pageFile.write(&data.at(writtenBytes), page.used);
            id = page.next;
            pageIndex++;
        }
    });
}

void MSG::PageFile::Shrink()
{
    PageCount newSize = _pages.size();
    for (auto& page : std::ranges::reverse_view(_pages)) {
        if (page.used == 0)
            newSize--;
    }
    _Resize(newSize);
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
    _thread.PushCommand([this, newSize = a_Size] {
        std::filesystem::resize_file(_pageFilePath, newSize * PageSize);
    });
}

size_t MSG::PageFile::_GetSize()
{
    _thread.Wait();
    return _pages.size();
}
