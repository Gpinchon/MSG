#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstring>
#include <cstdint>
#include <array>
#include <memory>
#include <type_traits>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Tools {
/**
* @brief sizeof(SparseSet) is at least sizeof(Type) * Size. Large sets should
* therefore be allocated on the heap.
* Every time an element is erased invalidates every object reference to elements
* in this set.
* In general it is ill-advised to keep reference to objects inside the set.
* Users should instead reference the set and access elements through index when
* they need it.
*/
template<typename Type, uint32_t Size>
class SparseSet {
public:
    using value_type = Type;
    using size_type = decltype(Size);

    constexpr SparseSet() noexcept;
    inline ~SparseSet() noexcept(std::is_nothrow_invocable_v<decltype(&SparseSet::clear), SparseSet>);

    /** @return The maximum number of elements that can be inserted in the set*/
    [[nodiscard]] constexpr size_type max_size() const noexcept;
    /** @return The number of elements contained in the set */
    [[nodiscard]] constexpr size_type size() const noexcept;
    /** @return true if the set contains no element */
    [[nodiscard]] constexpr bool empty() const noexcept;
    /** @return true if the number of elements in the set equals max_size() */
    [[nodiscard]] constexpr bool full() const noexcept;
    /** @brief empties the set */
    constexpr void clear()
        noexcept(std::is_nothrow_invocable_v<decltype(&SparseSet::erase), SparseSet, size_type > );

    /** @return a ref to the element contained at this index */
    [[nodiscard]] constexpr value_type& at(size_type a_Index);
    /** @return a ref to the element contained at this index */
    [[nodiscard]] constexpr const value_type& at(size_type a_Index) const;

    /** @return *UNCHECKED* a ref to the element contained at this index */
    [[nodiscard]] constexpr value_type& operator[](size_type a_Index) noexcept;
    /** @return *UNCHECKED* a ref to the element contained at this index */
    [[nodiscard]] constexpr const value_type& operator[](size_type a_Index) const noexcept;

    /**
    * @brief Inserts a new element at the specified index,
    * replaces the current element if it already exists
    * @return a ref to the newly created element
    */
    template<typename ...Args>
    constexpr value_type& insert(size_type a_Index, Args&&... a_Args)
        noexcept(std::is_nothrow_constructible_v<value_type, Args...> && std::is_nothrow_destructible_v<value_type>);
    /** @brief Removes the element at the specified index */
    constexpr void erase(size_type a_Index)
        noexcept(std::is_nothrow_destructible_v<value_type>);
    /** @return true if a value is attached to this index */
    constexpr bool contains(size_type a_Index) const;

private:
#pragma warning(push)
#pragma warning(disable : 26495) //variables are left uninitialized on purpose
    struct Storage {
        size_type                       sparseIndex;
        alignas(value_type) std::byte   data[sizeof(value_type)];
        operator value_type& () { return *(value_type*)data; }
    };
#pragma warning(pop)
    size_type _size{ 0 };
    std::array<size_type, Size> _sparse;
    std::array<Storage, Size>   _dense;
};

template<typename Type, uint32_t Size>
constexpr SparseSet<Type, Size>::SparseSet() noexcept {
    _sparse.fill(max_size());
}

template<typename Type, uint32_t Size>
inline SparseSet<Type, Size>::~SparseSet()
     noexcept(std::is_nothrow_invocable_v<decltype(&SparseSet::clear), SparseSet>)
{
    clear();
}

template<typename Type, uint32_t Size>
constexpr auto SparseSet<Type, Size>::max_size() const noexcept -> size_type {
    return Size;
}

template<typename Type, uint32_t Size>
constexpr auto SparseSet<Type, Size>::size() const noexcept -> size_type {
    return _size;
}

template<typename Type, uint32_t Size>
constexpr bool SparseSet<Type, Size>::empty() const noexcept {
    return _size == 0;
}

template<typename Type, uint32_t Size>
constexpr bool SparseSet<Type, Size>::full() const noexcept {
    return _size == max_size();
}

template<typename Type, uint32_t Size>
constexpr void SparseSet<Type, Size>::clear()
    noexcept(std::is_nothrow_invocable_v<decltype(&SparseSet::erase), SparseSet, size_type>)
{
    for (size_type index = 0; !empty(); ++index) {
        erase(index);
    }
}

template<typename Type, uint32_t Size>
constexpr auto SparseSet<Type, Size>::at(size_type a_Index) -> value_type& {
    return _dense.at(_sparse.at(a_Index));
}

template<typename Type, uint32_t Size>
constexpr auto SparseSet<Type, Size>::at(size_type a_Index) const -> const value_type& {
    return _dense.at(_sparse.at(a_Index));
}

template<typename Type, uint32_t Size>
constexpr auto SparseSet<Type, Size>::operator[](size_type a_Index) noexcept -> value_type& {
    return _dense[_sparse[a_Index]];
}

template<typename Type, uint32_t Size>
constexpr auto SparseSet<Type, Size>::operator[](size_type a_Index) const noexcept -> const value_type& {
    return _dense[_sparse[a_Index]];
}

template<typename Type, uint32_t Size>
template<typename ...Args>
constexpr auto SparseSet<Type, Size>::insert(size_type a_Index, Args && ...a_Args)
    noexcept(std::is_nothrow_constructible_v<value_type, Args...> && std::is_nothrow_destructible_v<value_type>) -> value_type&
{
    if (contains(a_Index)) //just replace the element
    {
        auto& dense = _dense[_sparse[a_Index]];
        std::destroy_at((value_type*)dense.data);
        return *new(dense.data) value_type(std::forward<Args>(a_Args)...);;
    }
    //Push new element back
    _sparse[a_Index] = _size;
    auto& dense = _dense.at(_size); //if full it should crash here
    dense.sparseIndex = a_Index;
    _size++;
    return *new(dense.data) value_type(std::forward<Args>(a_Args)...);
}

template<typename Type, uint32_t Size>
constexpr void SparseSet<Type, Size>::erase(size_type a_Index)
    noexcept(std::is_nothrow_destructible_v<value_type>)
{
    if (empty() || !contains(a_Index)) return;
    _size--;
    auto& currDense = _dense[_sparse[a_Index]];
    auto& lastDense = _dense[_size];
    size_type lastIndex = lastDense.sparseIndex;
    std::destroy_at((value_type*)currDense.data); //call current data's destructor
    std::memmove(currDense.data, lastDense.data, sizeof(value_type)); //crush current data with last data
    std::swap(lastDense.sparseIndex, currDense.sparseIndex);
    std::swap(_sparse[lastIndex], _sparse[a_Index]);
    _sparse[a_Index] = max_size();
}

template<typename Type, uint32_t Size>
constexpr bool SparseSet<Type, Size>::contains(size_type a_Index) const {
    //if a_Index is out of bound we should crash here
    return _sparse.at(a_Index) != max_size();
}
}
