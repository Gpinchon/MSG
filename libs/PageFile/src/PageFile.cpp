#include <MSG/PageFile.hpp>

MSG::PageFileMemoryRange::~PageFileMemoryRange()
{
    _pageFile._Release(*this);
}

MSG::PageFileMappedRange* MSG::PageFileMemoryRange::Map(const PageFileOffset& a_Offset, const PageFileSize& a_Size)
{
    return nullptr;
}