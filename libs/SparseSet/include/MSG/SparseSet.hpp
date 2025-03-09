#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <array>
#include <cstdint>
#include <cstring>
#include <memory>
#include <type_traits>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
/**
 * @brief sizeof(SparseSet) is at least sizeof(Type) * Size. Large sets should
 * therefore be allocated on the heap.
 * Every time an element is erased invalidates every object reference to elements
 * in this set.
 * In general it is ill-advised to keep reference to objects inside the set.
 * Users should instead reference the set and access elements through index when
 * they need it.
 */
template <typename Type, size_t Size>
class SparseSet {
public:
    using value_type = Type;
    using size_type  = decltype(Size);

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
    constexpr void clear() noexcept(std::is_nothrow_invocable_v<decltype(&SparseSet::erase), SparseSet, size_type>);

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
    template <typename... Args>
    constexpr value_type& insert(size_type a_Index, Args&&... a_Args) noexcept(std::is_nothrow_constructible_v<value_type, Args...> && std::is_nothrow_destructible_v<value_type>);
    /** @brief Removes the element at the specified index */
    constexpr void erase(size_type a_Index) noexcept(std::is_nothrow_destructible_v<value_type>);
    /** @return true if a value is attached to this index */
    constexpr bool contains(size_type a_Index) const;

private:
#pragma warning(push)
#pragma warning(disable : 26495) // variables are left uninitialized on purpose
    struct Storage {
        size_type sparseIndex;
        alignas(value_type) std::byte data[sizeof(value_type)];
        operator value_type&() { return *reinterpret_cast<value_type*>(data); }
    };
#pragma warning(pop)
    size_type _size { 0 };
    std::array<size_type, Size> _sparse;
    std::array<Storage, Size> _dense;
};
}

#include <MSG/SparseSet.inl>
