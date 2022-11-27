#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <cstddef>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Tools {
template<typename Type, uint32_t Size>
class FixedSizeMemoryPool {
public:
    static_assert(sizeof(Type) >= sizeof(uint32_t));
    class Deleter {
    public:
        Deleter(FixedSizeMemoryPool& a_Pool) : _memoryPool(a_Pool) {}
        void operator()(Type* const a_Ptr) { _memoryPool.deallocate(a_Ptr); }

    private:
        FixedSizeMemoryPool& _memoryPool;
    };
    typedef Type value_type;
    typedef uint32_t size_type;
    typedef ptrdiff_t difference_type;

    template<typename U> struct rebind { typedef FixedSizeMemoryPool<U, Size> other; };

    FixedSizeMemoryPool() {
        std::memset(_memory, 0, sizeof(_memory));
        *reinterpret_cast<size_type*>(_next) = 0;
    }
    FixedSizeMemoryPool(FixedSizeMemoryPool&& a_Other)
        : _cellNumUsed(std::move(a_Other._cellNumUsed))
        , _cellNumFree(std::move(a_Other._cellNumFree))
        , _memory(std::move(a_Other._memory))
        , _next(std::move(a_Other._next))
    {}
    FixedSizeMemoryPool(const FixedSizeMemoryPool& a_Other) noexcept : FixedSizeMemoryPool() {}
    template<typename U>
    FixedSizeMemoryPool(const FixedSizeMemoryPool<U, Size>&) noexcept : FixedSizeMemoryPool() {}

    Type* allocate() {
        if (_cellNumUsed < _cellNum) {
            auto p = (size_type*)addr_from_index(_cellNumUsed);
            *p = ++_cellNumUsed;
        }
        Type* res = nullptr;
        if (_cellNumFree > 0)
        {
            res = (Type*)_next;
            if (--_cellNumFree > 0)
                _next = addr_from_index(*(uint32_t*)_next);
            else _next = nullptr;
        }
        return res;
    }
    void deallocate(Type* const a_Ptr) noexcept {
        if (_next != nullptr) {
            *(size_type*)a_Ptr = index_from_addr(_next);
            _next = (std::byte*)a_Ptr;
        }
        else {
            *(size_type*)a_Ptr = _cellNum;
            _next = (std::byte*)a_Ptr;
        }
        ++_cellNumFree;
    }

    bool empty() const {
        return _cellNumFree == _cellNum;
    }
    size_type index_from_addr(std::byte* a_Ptr) const {
        return size_type(a_Ptr - _memory) / _cellSize;
    }
    std::byte* addr_from_index(size_type a_Index) {
        return _memory + (a_Index * _cellSize);
    }
    auto deleter() noexcept {
        return Deleter(*this);
    }
    constexpr size_type max_size() const noexcept {
        return Size;
    }
    size_type count() const noexcept {
        return Size - _cellNumFree;
    }
    size_type free() const noexcept {
        return _cellNumFree;
    }

    template<typename U>
    bool operator!=(const FixedSizeMemoryPool<U, Size>& a_Right) { return false; }
    template<typename U>
    bool operator==(const  FixedSizeMemoryPool<U, Size>& a_Right) { return !(*this != a_Right); }

private:
    static constexpr auto   _cellSize = sizeof(Type);
    static constexpr auto   _cellNum = Size;
    size_type               _cellNumUsed{ 0 };
    size_type               _cellNumFree{ _cellNum };
    std::byte               _memory[Size * sizeof(Type)]{};
    std::byte* _next{ &_memory[0] };
};
}
