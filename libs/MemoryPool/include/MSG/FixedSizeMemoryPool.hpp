#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <utility>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
template <typename Type, uint32_t Size>
class FixedSizeMemoryPool {
public:
    typedef Type value_type;
    typedef uint32_t size_type;
    typedef ptrdiff_t difference_type;
    static_assert(sizeof(value_type) >= sizeof(size_type));

    template <typename U>
    struct rebind {
        typedef FixedSizeMemoryPool<U, Size> other;
    };
    class Deleter {
    public:
        constexpr Deleter(FixedSizeMemoryPool& a_Pool)
            : _memoryPool(a_Pool)
        {
        }
        constexpr void operator()(Type* const a_Ptr)
        {
            std::destroy_at(a_Ptr);
            _memoryPool.deallocate(a_Ptr);
        }

    private:
        FixedSizeMemoryPool& _memoryPool;
    };

    inline constexpr FixedSizeMemoryPool() noexcept;
    inline constexpr FixedSizeMemoryPool(FixedSizeMemoryPool&& a_Other) noexcept;
    inline constexpr FixedSizeMemoryPool(const FixedSizeMemoryPool& a_Other) noexcept
        : FixedSizeMemoryPool()
    {
    }
    template <typename U>
    inline constexpr FixedSizeMemoryPool(const FixedSizeMemoryPool<U, Size>&) noexcept
        : FixedSizeMemoryPool()
    {
    }

    /** @return the pointer to a newly allocated object */
    inline constexpr value_type* allocate() noexcept;
    /** @brief returns the space of the pointer to the pool */
    inline constexpr void deallocate(value_type* const a_Ptr) noexcept;

    /** @return true if this pool contains no object */
    inline constexpr bool empty() const noexcept;
    /** @return the count of objects contained in this pool */
    inline constexpr size_type count() const noexcept;
    /** @return the number of available slots in this pool */
    inline constexpr size_type free() const noexcept;
    /** @return the maximum number of object this pool can contain */
    inline constexpr size_type max_size() const noexcept;

    /** @return a deleter constructed from this pool */
    inline constexpr Deleter deleter() noexcept;

    inline constexpr size_type index_from_addr(std::byte* a_Ptr) const noexcept;
    inline constexpr std::byte* addr_from_index(size_type a_Index) noexcept;
    inline constexpr const std::byte* addr_from_index(size_type a_Index) const noexcept;

    template <typename U>
    bool operator!=(const FixedSizeMemoryPool<U, Size>& a_Right) { return false; }
    template <typename U>
    bool operator==(const FixedSizeMemoryPool<U, Size>& a_Right) { return !(*this != a_Right); }

private:
    size_type _cellNumUsed { 0 };
    size_type _cellNumFree { max_size() };
    std::byte* _next { &_memory[0] };
    alignas(value_type) std::byte _memory[sizeof(value_type) * Size] {};
};
}

#include <MSG/FixedSizeMemoryPool.inl>
