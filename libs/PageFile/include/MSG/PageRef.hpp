#pragma once

#include <MSG/PageFile.hpp>

namespace Msg {
class PageRef {
public:
    PageRef(PageFile& a_PageFile, const PageID& a_PageID)
        : _pageFile(&a_PageFile)
        , _id(a_PageID)
    {
    }
    PageRef(PageRef&& a_Other)
        : _pageFile(a_Other._pageFile)
        , _id(a_Other._id)
    {
        _id       = NoPageID;
        _pageFile = nullptr;
    }
    PageRef(const PageRef&) = delete;
    ~PageRef()
    {
        if (_pageFile != nullptr)
            _pageFile->Release(_id);
    }
    operator PageID() const { return _id; }

private:
    PageID _id;
    PageFile* _pageFile;
};
}