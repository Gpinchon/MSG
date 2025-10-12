#pragma once

#include <MSG/Mesh/Vertex.hpp>

#include <memory>
#include <vector>

namespace Msg {
class PageRef;
}

namespace Msg {
class PrimitiveStorage {
public:
    void Resize(const size_t& a_NewSize);
    size_t GetSize() const { return _size; }
    std::vector<Vertex> Read(const size_t a_Offset, const size_t& a_Count) const;
    std::vector<Vertex> Read() const { return Read(0, _size); }
    void Write(const size_t a_Offset, std::vector<Vertex> a_Data);

private:
    size_t _size = 0;
    std::shared_ptr<PageRef> _pageRef;
};
}