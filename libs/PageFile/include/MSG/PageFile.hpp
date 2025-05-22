#pragma once

#include <MSG/WorkerThread.hpp>

#include <deque>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <vector>

namespace MSG {
using PageID              = uint32_t;
using PageCount           = uint32_t;
constexpr size_t PageSize = 4096u; // default size is 4Kb

struct Page {
    PageID next   = -1u; // pointer to the next data chunk
    uint32_t used = 0; // number of bytes used
};

class PageFile {
public:
    PageFile();
    PageFile(std::FILE* a_LibFile, std::FILE* a_PageFile);
    ~PageFile();
    template <typename T>
    PageID Allocate(const size_t& a_Count = 1);
    PageID Allocate(const size_t& a_ByteSize);
    void Release(const PageID& a_PageID);
    std::vector<std::byte> Read(const PageID& a_PageID, const size_t& a_ByteSize);
    void Write(const PageID& a_PageID, std::vector<std::byte>&& a_Data);
    void Shrink();

private:
    void _Resize(const PageCount& a_Count);
    size_t _GetSize();
    MSG::WorkerThread _thread;
    std::filesystem::path _libFilePath;
    std::filesystem::path _pageFilePath;
    std::vector<Page> _pages;
    std::deque<PageID> _freePages;
    std::basic_fstream<std::byte> _libFile;
    std::basic_fstream<std::byte> _pageFile;
};

template <typename T>
inline PageID MSG::PageFile::Allocate(const size_t& a_Count)
{
    return Allocate(RoundByteSize(sizeof(T) * a_Count));
}
}