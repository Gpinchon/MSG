#pragma once

#include <array>
#include <chrono>
#include <memory>
#include <queue>
#include <stddef.h>
#include <unordered_map>
#include <vector>

#include <Renderer/OGL/RAII/Buffer.hpp>

namespace TabGraph::Renderer {
struct Context;
}

namespace TabGraph::Renderer::RAII {
struct Buffer;
}

namespace TabGraph::Renderer {
class TextureBufferPoolBlock {
public:
    static constexpr uint8_t size             = 32;
    static constexpr double keepAliveDuration = 2000;
    explicit TextureBufferPoolBlock(Context& a_context, const size_t& a_Size);
    ~TextureBufferPoolBlock();
    uint8_t GetIndex();
    void ReleaseIndex(const uint8_t& a_Index);
    bool HasFreeIndex() const;
    // returns true if _freeIndex is full and block was not used since keepAliveDuration
    bool NeedsFreeing() const;
    RAII::Buffer& at(const size_t& a_Index);

private:
    void _UpdateLastUse();
    Context& _context;
    // when was the last time we used this block
    std::chrono::time_point<std::chrono::system_clock> _lastUse = std::chrono::system_clock::now();
    std::queue<uint8_t> _freeIndice;
    std::array<RAII::Buffer*, size> _buffers;
    alignas(RAII::Buffer) std::array<std::byte, sizeof(RAII::Buffer) * size> _memory;
};

class TextureBufferPool {
public:
    struct BufferBlockIndex {
        size_t size;
        uint32_t blockIndex; // buffer block index inside pool vector
        uint32_t bufferIndex; // buffer index inside buffer block
    };
    TextureBufferPool(Context& a_Context);
    std::shared_ptr<RAII::Buffer> Allocate(const size_t& a_Size);
    // free the unused blocks
    void Update();

private:
    void _AllocateBlock(const size_t& a_Size);
    void _Free(const BufferBlockIndex& a_Index);
    Context& _context;
    std::unordered_map<size_t, std::vector<uint32_t>> _freeBlocks;
    std::unordered_map<size_t, std::vector<TextureBufferPoolBlock>> _blocks;
};
}