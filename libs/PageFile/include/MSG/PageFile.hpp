#pragma once

#include <cstdio>
#include <deque>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace MSG {
class PageFile;
using PageID              = size_t;
using PageCount           = size_t;
constexpr PageID NoPageID = -1u;
constexpr size_t PageSize = 4096u; // default size is 4Kb

class PageFile {
public:
    PageCount RoundByteSize(const size_t& a_ByteSize);
    static PageFile& Global();
    PageFile();
    ~PageFile();
    auto& GetLock() { return _mtx; }
    template <typename T>
    [[nodiscard]] PageID Allocate(const size_t& a_Count = 1);
    /**
     * @brief Allocates the number of pages required to store the specified byte size
     * @attention The memory pages are not always contiguous.
     * Do not assume PageID+1 will belong to the memory range you just allocated
     *
     * @param a_ByteSize the byte size to allocate
     * @return PageID the id of the first page for the newly allocated range.
     */
    [[nodiscard]] PageID Allocate(const size_t& a_ByteSize);
    /**
     * @brief Releases a memory range and store its pages inside the free pages list
     * @attention if any mapped memory exists for this memory range it will be deleted.
     * This invalidates the vector reference returned by Map
     *
     * @param a_PageID the id of the memory range to release
     */
    void Release(const PageID& a_PageID);
    /**
     * @brief Reads the specified byte range to a bytes vector
     *
     * @param a_PageID the id of the memory range to read
     * @param a_ByteOffset the byte offset inside the memory range to read
     * @param a_ByteSize the byte size of the memory range to read
     * @return std::vector<std::byte> : a vector of bytes containing the data of the specified memory range
     */
    [[nodiscard]] std::vector<std::byte> Read(const PageID& a_PageID, const size_t& a_ByteOffset, const size_t& a_ByteSize);
    /**
     * @brief Writes the specified data to the memory range
     *
     * @param a_PageID the id of the memory range to write to
     * @param a_ByteOffset the byte offset inside the specified memory range
     * @param a_Data the data to write
     */
    void Write(const PageID& a_PageID, const size_t& a_ByteOffset, std::vector<std::byte> a_Data);
    /**
     * @brief Remove the trailing pages and resize the page file accordingly
     *
     */
    void Shrink();

private:
    struct Page {
        PageID next   = NoPageID; // pointer to the next data chunk
        uint32_t used = 0; // number of bytes used
    };
    void _Resize(const PageCount& a_Count);
    std::recursive_mutex _mtx;
    std::vector<Page> _pages;
    std::deque<PageID> _freePages;
    std::FILE* _pageFile;
};

template <typename T>
inline PageID MSG::PageFile::Allocate(const size_t& a_Count)
{
    return Allocate(RoundByteSize(sizeof(T) * a_Count));
}
}