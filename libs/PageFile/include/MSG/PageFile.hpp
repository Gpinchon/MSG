#pragma once

#include <cstddef>
#include <filesystem>
#include <span>
#include <unordered_map>
#include <vector>

namespace MSG {
using PageFileOffset    = size_t;
using PageFileSize      = size_t;
using PageFileAlignment = size_t;
class PageFile;
class PageFileMemoryRange;

class PageFileMappedRange {
public:
    ~PageFileMappedRange();

private:
    friend PageFileMemoryRange;
    PageFileMappedRange(const PageFileOffset& a_Offset, const PageFileSize& a_Size, const std::byte* a_Data);
    const PageFileOffset _offset;
    std::span<std::byte> _mappedMemory;
    PageFileMemoryRange& _memoryRange;
};

class PageFileMemoryRange {
public:
    ~PageFileMemoryRange();
    PageFileMappedRange* Map(const PageFileOffset& a_Offset, const PageFileSize& a_Size);
    void Unmap();

private:
    friend PageFile;
#ifndef _NDEBUG
    std::vector<std::pair<PageFileOffset, PageFileSize>> _mappedRanges;
#endif
    PageFileOffset _offset;
    PageFileSize _size;
    PageFileAlignment _alignment;
    PageFile& _pageFile;
};

class PageFile {
public:
    PageFile(const std::FILE* a_File = std::tmpfile());
    template <typename T>
    PageFileMemoryRange* Allocate(const size_t& a_Count = 1);
    PageFileMemoryRange* Allocate(const PageFileSize& a_Size, const PageFileAlignment& a_Alignment);

private:
    friend PageFileMemoryRange;
    void _Release(const PageFileMemoryRange& a_Range);
    std::FILE* _file;
    std::vector<std::pair<PageFileOffset, PageFileSize>> _freeRanges;
};

template <typename T>
inline PageFileMemoryRange* MSG::PageFile::Allocate(const size_t& a_Count)
{
    return Allocate(sizeof(T) * a_Count, alignof(T));
}
}