#include <Renderer/OGL/TextureBufferPool.hpp>

#include <Renderer/OGL/RAII/Buffer.hpp>

#include <Renderer/OGL/Unix/Context.hpp>

#include <cassert>
#include <climits>
#include <type_traits>

namespace TabGraph::Renderer {
// taken from https://stackoverflow.com/a/27915457
template <typename UnsignedType>
UnsignedType ToNextPow2(UnsignedType v)
{
    static_assert(std::is_unsigned_v<UnsignedType>, "Only works for unsigned types");
    v--;
    for (size_t i = 1; i < sizeof(v) * CHAR_BIT; i *= 2) // Prefer size_t "Warning comparison between signed and unsigned integer"
    {
        v |= v >> i;
    }
    return ++v;
}

TextureBufferPoolBlock::TextureBufferPoolBlock(Context& a_context, const size_t& a_Size)
    : _context(a_context)
{
    _context.PushImmediateCmd([this, a_Size] {
        for (auto i = 0u; i < size; ++i) {
            auto placementPtr = (RAII::Buffer*)_memory.at(i * sizeof(RAII::Buffer));
            _buffers.at(i)    = new (placementPtr) RAII::Buffer(a_Size, nullptr, 0);
            _freeIndice.push(i);
        }
    });
}

TextureBufferPoolBlock::~TextureBufferPoolBlock()
{
    _context.PushCmd([memory = _memory]() mutable {
        auto buffersCount = memory.size() / sizeof(RAII::Buffer);
        auto buffers      = (RAII::Buffer*)memory.data();
        for (auto i = 0u; i < buffersCount; ++i) {
            delete &buffers[i];
        }
    });
}

uint8_t TextureBufferPoolBlock::GetIndex()
{
    auto index = _freeIndice.back();
    _freeIndice.pop();
    return index;
}

void TextureBufferPoolBlock::ReleaseIndex(const uint8_t& a_Index)
{
    _UpdateLastUse();
    _freeIndice.push(a_Index);
}

bool TextureBufferPoolBlock::HasFreeIndex() const
{
    return !_freeIndice.empty();
}

bool TextureBufferPoolBlock::NeedsFreeing() const
{
    return _freeIndice.size() == size
        && std::chrono::duration<double, std::milli>(std::chrono::system_clock::now() - _lastUse).count() > keepAliveDuration;
}

RAII::Buffer& TextureBufferPoolBlock::at(const size_t& a_Index)
{
    return *_buffers.at(a_Index);
}

void TabGraph::Renderer::TextureBufferPoolBlock::_UpdateLastUse()
{
    _lastUse = std::chrono::system_clock::now();
}

TextureBufferPool::TextureBufferPool(Context& a_Context)
    : _context(a_Context)
{
}

std::shared_ptr<RAII::Buffer> TextureBufferPool::Allocate(const size_t& a_Size)
{
    assert(a_Size > 0);
    auto size        = ToNextPow2(a_Size);
    auto freeBlockIt = _freeBlocks.find(size);
    std::shared_ptr<RAII::Buffer> buffer;
    if (freeBlockIt != _freeBlocks.end() && !freeBlockIt->second.empty()) {
        // we have a pool !
        auto blockIndex              = freeBlockIt->second.back();
        auto& block                  = _blocks.at(size).at(blockIndex);
        BufferBlockIndex bufferIndex = { size, blockIndex, block.GetIndex() };
        buffer                       = std::shared_ptr<RAII::Buffer>(
            &_blocks.at(size).at(bufferIndex.blockIndex).at(bufferIndex.bufferIndex),
            [this, bufferIndex](RAII::Buffer const*) {
                _Free(bufferIndex);
            });
        if (!block.HasFreeIndex())
            freeBlockIt->second.pop_back();
    } else {
        // no more free buffers for this size
        _AllocateBlock(size);
        buffer = Allocate(size);
    }
    return buffer;
}

void TextureBufferPool::Update()
{
    // std::vector<std::pair<size_t, uint32_t>> blocksToFree;
    // for (auto& freeBlockVec : _freeBlocks) {
    //     for (auto& freeBlockIndex : freeBlockVec.second) {
    //         auto block = _blocks.at(freeBlockVec.first).at(freeBlockIndex);
    //         if (block.NeedsFreeing())
    //             blocksToFree
    //     }
    // }
}

void TextureBufferPool::_AllocateBlock(const size_t& a_Size)
{
    _context.PushImmediateCmd([this, a_Size]() {
        auto& blockVec = _blocks[a_Size];
        blockVec.emplace_back(_context, a_Size);
        _freeBlocks[a_Size].push_back(blockVec.size());
    });
}

void TextureBufferPool::_Free(const BufferBlockIndex& a_Index)
{
    _blocks.at(a_Index.size).at(a_Index.blockIndex).ReleaseIndex(a_Index.bufferIndex);
}
}