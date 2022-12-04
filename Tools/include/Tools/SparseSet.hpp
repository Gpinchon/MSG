#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <array>
#include <memory>
#include <type_traits>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Tools {
template<typename Type, uint32_t Size>
class SparseSet {
public:
    using value_type = Type;
    using size_type = decltype(Size);

    constexpr inline SparseSet();
    inline ~SparseSet();

    /** @return The maximum number of elements that can be inserted in the set*/
    constexpr inline size_type max_size() const noexcept;
    /** @return The number of elements contained in the set */
    constexpr inline size_type size() const noexcept;
    /** @return true if the set contains no element */
    constexpr inline bool empty() const noexcept;
    /** @return true if the number of elements in the set equals max_size() */
    constexpr inline bool full() const noexcept;

    /** @return the element contained at this index */
    constexpr inline value_type& at(size_type a_Index);
    /** @return the element contained at this index */
    constexpr inline const value_type& at(size_type a_Index) const;

    /**
    * @brief Inserts a new element at the specified index,
    * replaces the current element if it already exists
    * @return the newly created element
    */
    template<typename ...Args>
    constexpr inline value_type& insert(size_type a_Index, Args&&... a_Args)
        noexcept(std::is_nothrow_constructible_v<value_type, Args...> && std::is_nothrow_destructible_v<value_type>);
    /** @brief Removes the element at the specified index */
    constexpr inline void erase(size_type a_Index)
        noexcept(std::is_nothrow_destructible_v<value_type>);
    /** @return true if a value is attached to this index */
    constexpr inline bool contains(size_type a_Index) const;

private:
#pragma warning(push)
#pragma warning(disable : 26495) //variables are left unitinitlized on purpose
    struct Storage {
        size_type						sparseIndex;
        alignas(value_type) std::byte	data[sizeof(value_type)];
        operator value_type& () { return *(value_type*)data; }
    };
#pragma warning(pop)
    size_type _size{ 0 };
    std::array<size_type, Size> _sparse;
    std::array<Storage, Size>   _dense;
};

template<typename Type, uint32_t Size>
inline constexpr SparseSet<Type, Size>::SparseSet() {
    _sparse.fill(max_size());
}

template<typename Type, uint32_t Size>
inline SparseSet<Type, Size>::~SparseSet() {
    for (size_type index = 0; !empty(); ++index) {
        if (contains(index)) erase(index);
    }
}

template<typename Type, uint32_t Size>
inline constexpr auto SparseSet<Type, Size>::max_size() const noexcept -> size_type {
    return Size;
}

template<typename Type, uint32_t Size>
inline constexpr auto SparseSet<Type, Size>::size() const noexcept -> size_type {
    return _size;
}

template<typename Type, uint32_t Size>
inline constexpr bool SparseSet<Type, Size>::empty() const noexcept {
    return _size == 0;
}

template<typename Type, uint32_t Size>
inline constexpr bool SparseSet<Type, Size>::full() const noexcept {
    return _size == max_size();
}

template<typename Type, uint32_t Size>
inline constexpr auto SparseSet<Type, Size>::at(size_type a_Index) -> value_type& {
    return _dense.at(_sparse.at(a_Index));
}

template<typename Type, uint32_t Size>
inline constexpr auto SparseSet<Type, Size>::at(size_type a_Index) const -> const value_type& {
    return _dense.at(_sparse.at(a_Index));
}

template<typename Type, uint32_t Size>
template<typename ...Args>
inline constexpr auto SparseSet<Type, Size>::insert(size_type a_Index, Args && ...a_Args)
    noexcept(std::is_nothrow_constructible_v<value_type, Args...> && std::is_nothrow_destructible_v<value_type>) -> value_type& {
    if (contains(a_Index)) //just replace the element
    {
        auto& dense = _dense.at(_sparse.at(a_Index));
        std::destroy_at((value_type*)dense.data);
        new(&dense.data) value_type(std::forward<Args>(a_Args)...);
        return (value_type&)dense;
    }
    //Push new element back
    auto& dense = _dense.at(_size);
    new(&dense.data) value_type(std::forward<Args>(a_Args)...);
    dense.sparseIndex = a_Index;
    _size++;
    _sparse.at(a_Index) = _size - 1;
    return (value_type&)dense;
}

template<typename Type, uint32_t Size>
inline constexpr void SparseSet<Type, Size>::erase(size_type a_Index) noexcept(std::is_nothrow_destructible_v<value_type>) {
    if (empty() || !contains(a_Index)) return;
    auto& currDense = _dense.at(_sparse.at(a_Index));
    auto& lastDense = _dense.at(_size - 1);
    size_type lastIndex = lastDense.sparseIndex;
    std::destroy_at((value_type*)currDense.data); //call current data's destructor
    std::memmove(currDense.data, lastDense.data, sizeof(value_type)); //crush current data with last data
    std::swap(lastDense.sparseIndex, currDense.sparseIndex);
    std::swap(_sparse.at(lastIndex), _sparse.at(a_Index));
    _sparse.at(a_Index) = max_size();
    _size--;
}

/**@return true if a value is attached to this index */
template<typename Type, uint32_t Size>
inline constexpr bool SparseSet<Type, Size>::contains(size_type a_Index) const {
    return _sparse.at(a_Index) != max_size();
}
}
