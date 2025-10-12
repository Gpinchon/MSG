#include <MSG/Mesh/PrimitiveStorage.hpp>

#include <MSG/PageFile.hpp>
#include <MSG/PageRef.hpp>

void Msg::PrimitiveStorage::Resize(const size_t& a_NewSize)
{
    if (a_NewSize != _size) {
        _pageRef.reset(); // free this pagefile in case we can reuse it
        _pageRef = std::make_shared<PageRef>(PageFile::Global(), PageFile::Global().Allocate(sizeof(Vertex) * a_NewSize));
        _size    = a_NewSize;
    }
}

std::vector<Msg::Vertex> Msg::PrimitiveStorage::Read(const size_t a_Offset, const size_t& a_Count) const
{
    auto data = PageFile::Global().Read(*_pageRef, sizeof(Vertex) * a_Offset, sizeof(Vertex) * a_Count);
    return {
        reinterpret_cast<Vertex*>(std::to_address(data.begin())),
        reinterpret_cast<Vertex*>(std::to_address(data.end())),
    };
}

void Msg::PrimitiveStorage::Write(const size_t a_Offset, std::vector<Vertex> a_Data)
{
    PageFile::Global().Write(*_pageRef, sizeof(Vertex) * a_Offset,
        {
            reinterpret_cast<std::byte*>(std::to_address(a_Data.begin())),
            reinterpret_cast<std::byte*>(std::to_address(a_Data.end())),
        });
}
