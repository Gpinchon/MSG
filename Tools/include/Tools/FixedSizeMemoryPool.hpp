#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstddef>
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Tools {
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
        constexpr void operator()(Type* const a_Ptr) { _memoryPool.deallocate(a_Ptr); }

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

template <typename Type, uint32_t Size>
inline constexpr FixedSizeMemoryPool<Type, Size>::FixedSizeMemoryPool() noexcept
{
    std::memset(_memory, 0, sizeof(_memory));
    *reinterpret_cast<size_type*>(_next) = 0;
}

template <typename Type, uint32_t Size>
inline constexpr FixedSizeMemoryPool<Type, Size>::FixedSizeMemoryPool(FixedSizeMemoryPool&& a_Other) noexcept
    : _cellNumUsed(std::move(a_Other._cellNumUsed))
    , _cellNumFree(std::move(a_Other._cellNumFree))
    , _memory(std::move(a_Other._memory))
    , _next(std::move(a_Other._next))
{
}

template <typename Type, uint32_t Size>
inline constexpr auto FixedSizeMemoryPool<Type, Size>::allocate() noexcept -> value_type*
{
    if (_cellNumUsed < max_size()) {
        auto p = (size_type*)addr_from_index(_cellNumUsed);
        *p     = ++_cellNumUsed;
    }
    Type* res = nullptr;
    if (_cellNumFree > 0) {
        res = (Type*)_next;
        if (--_cellNumFree > 0)
            _next = addr_from_index(*(uint32_t*)_next);
        else
            _next = nullptr;
    }
    return res;
}

template <typename Type, uint32_t Size>
inline constexpr void FixedSizeMemoryPool<Type, Size>::deallocate(value_type* const a_Ptr) noexcept
{
    if (_next != nullptr) {
        *(size_type*)a_Ptr = index_from_addr(_next);
        _next              = (std::byte*)a_Ptr;
    } else {
        *(size_type*)a_Ptr = max_size();
        _next              = (std::byte*)a_Ptr;
    }
    ++_cellNumFree;
}

template <typename Type, uint32_t Size>
inline constexpr bool FixedSizeMemoryPool<Type, Size>::empty() const noexcept
{
    return _cellNumFree == max_size();
}

template <typename Type, uint32_t Size>
inline constexpr auto FixedSizeMemoryPool<Type, Size>::index_from_addr(std::byte* a_Ptr) const noexcept -> size_type
{
    return size_type(a_Ptr - _memory) / sizeof(value_type);
}

template <typename Type, uint32_t Size>
inline constexpr std::byte* FixedSizeMemoryPool<Type, Size>::addr_from_index(size_type a_Index) noexcept
{
    return _memory + (a_Index * sizeof(value_type));
}

template <typename Type, uint32_t Size>
inline constexpr const std::byte* FixedSizeMemoryPool<Type, Size>::addr_from_index(size_type a_Index) const noexcept
{
    return _memory + (a_Index * sizeof(value_type));
}

template <typename Type, uint32_t Size>
inline constexpr auto FixedSizeMemoryPool<Type, Size>::deleter() noexcept -> Deleter
{
    return Deleter(*this);
}

template <typename Type, uint32_t Size>
inline constexpr auto FixedSizeMemoryPool<Type, Size>::count() const noexcept -> size_type
{
    return max_size() - _cellNumFree;
}

template <typename Type, uint32_t Size>
inline constexpr auto FixedSizeMemoryPool<Type, Size>::free() const noexcept -> size_type
{
    return _cellNumFree;
}

template <typename Type, uint32_t Size>
inline constexpr auto FixedSizeMemoryPool<Type, Size>::max_size() const noexcept -> size_type
{
    return Size;
}
}
