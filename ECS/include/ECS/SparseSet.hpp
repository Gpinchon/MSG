#pragma once

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
#include <Tools/FixedSizeMemoryPool.hpp>

#include <cstdint>
#include <array>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
template<typename Type, uint32_t MaxSize>
class SparseSet {
public:
    using value_type = Type;
    using size_type = decltype(MaxSize);
    

    inline SparseSet() { _sparse.fill(max_size()); }
    inline ~SparseSet() {
        for (size_type index = 0; !empty(); ++index) {
            if (contains(index)) erase(index);
        }
    }

    inline value_type& at(size_type a_Index) { return _dense[_sparse.at(a_Index)]; }
    inline value_type& at(size_type a_Index) const { return _dense[_sparse.at(a_Index)]; }
    inline auto size() const { return _size; }
    inline auto empty() const { return _size == 0; }
    inline auto full() const { return _size == max_size(); }
    constexpr size_type max_size() const noexcept { return MaxSize; }
    

    /**@brief inserts a new element at the specified index */
    template<typename ...Args>
    inline auto& insert(size_type a_Index, Args&&... a_Args) {
#ifdef _DEBUG
        assert(!contains(a_Index) && "Value already set at this index");
#endif
        //Push new element back
        {
            auto& dense = _dense[_size];
            new(&dense.data) value_type(std::forward<Args>(a_Args)...);
            dense.sparseIndex = a_Index;
            _size++;
        }
        _sparse.at(a_Index) = _size - 1;
        return (value_type&)_dense[_size - 1];
    }
    /** @brief removes the element at the specified index */
    inline void erase(size_type a_Index) {
#ifdef _DEBUG
        assert(contains(a_Index) && "No value set at this index");
#endif
        const auto last = _dense[_size - 1].sparseIndex;
        auto& currData = _dense[_sparse[a_Index]].data;
        auto& lastData = _dense[_size - 1].data;
        std::destroy_at((value_type*)currData); //call current data's destructor
        std::memmove(currData, lastData, sizeof(value_type)); //crush current data with last data
        std::swap(_dense[_size - 1].sparseIndex, _dense[_sparse[a_Index]].sparseIndex);
        std::swap(_sparse[last], _sparse[a_Index]);
        _sparse[a_Index] = max_size();
        _size--;
    }
    /**@return true if a value is attached to this index */
    inline auto contains(size_type a_Index) const {
        return _sparse.at(a_Index) != max_size();
    }

private:
#pragma warning(push)
#pragma warning(disable : 26495) //variables are left unitinitlized on purpose
    struct Storage {
        size_type						        sparseIndex;
        alignas(alignof(value_type)) std::byte	data[sizeof(value_type)];
        operator value_type& () { return *(value_type*)data; }
    };
#pragma warning(pop)
    size_type _size{ 0 };
    std::array<size_type, MaxSize> _sparse;
#pragma warning(push)
#pragma warning(disable : 26495) //variables are left unitinitlized on purpose
    std::array<Storage, MaxSize>   _dense;
#pragma warning(pop)
};
}
