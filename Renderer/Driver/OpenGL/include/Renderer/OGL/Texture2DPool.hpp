#pragma once

#include <Renderer/OGL/RAII/Texture.hpp>

#include <array>
#include <chrono>
#include <list>
#include <memory>
#include <queue>
#include <stddef.h>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

// TODO stop using Power of two and use texture views on 8k textures
// TODO use a quadtree to handle texture atlas subdivision

namespace TabGraph::Renderer {
struct Context;
}

namespace TabGraph::Renderer {
static constexpr auto TexturePoolMaxPowerOf2Exp = 13; // we support up to 8k resolution
static constexpr auto TexturePoolMaxLevels      = 32;
enum class TexturePoolClass {
    Unknown = -1,
    Size128Bits,
    Size96Bits,
    Size64Bits,
    Size48Bits,
    Size32Bits,
    Size24Bits,
    Size16Bits,
    Size8Bits,
    MaxValue
};

class Texture2DPoolBlock {
public:
    static constexpr uint8_t size             = 32;
    static constexpr double keepAliveDuration = 2000;
    explicit Texture2DPoolBlock(Context& a_context, const TexturePoolClass& a_TextureClass, const size_t& a_Size);
    ~Texture2DPoolBlock();
    uint8_t GetIndex();
    void ReleaseIndex(const uint8_t& a_Index);
    bool HasFreeIndex() const;
    // returns true if _freeIndex is full and block was not used since keepAliveDuration
    bool NeedsFreeing() const;
    RAII::Texture2D& at(const size_t& a_Index);
    const TexturePoolClass textureClass;
    const size_t& powerOf2Exp;

private:
    void _UpdateLastUse();
    Context& _context;
    // when was the last time we used this block
    std::chrono::time_point<std::chrono::system_clock> _lastUse = std::chrono::system_clock::now();
    std::queue<uint8_t> _freeIndice;
    std::array<RAII::Texture2D*, size> _textures;
    alignas(RAII::Texture2D) std::array<std::byte, sizeof(RAII::Texture2D) * size> _memory;
};

class Texture2DPool {
public:
    Texture2DPool(Context& a_Context);
    std::shared_ptr<RAII::Texture2D> Allocate(const uint& a_InternalFormat, const glm::uvec2& a_PowerOf2Exp, const uint& a_Levels);
    // free the unused blocks
    void Update();

private:
    void _AllocateBlock(const TexturePoolClass& a_TextureClass, const uint& a_PowerOf2Exp);
    void _Free(Texture2DPoolBlock* a_Block, const uint& a_TextureIndex);
    Context& _context;
    std::list<Texture2DPoolBlock> _blocks[unsigned(TexturePoolClass::MaxValue)][TexturePoolMaxPowerOf2Exp];
    std::list<Texture2DPoolBlock*> _freeBlocks[unsigned(TexturePoolClass::MaxValue)][TexturePoolMaxPowerOf2Exp];
};
}