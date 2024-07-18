#include <Renderer/OGL/Texture2DPool.hpp>

#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/Unix/Context.hpp>
#include <Tools/SequenceTable.hpp>

#include <cassert>
#include <climits>
#include <type_traits>

#include <GL/glew.h>

template <uint64_t N, typename T = uint64_t>
struct FactorialTraits : std::integral_constant<T, N * FactorialTraits<N - 1>::value> { };

namespace TabGraph::Renderer {
// taken from https://stackoverflow.com/a/27915457
template <typename UnsignedType>
UnsignedType NextPowerOf2(UnsignedType v)
{
    static_assert(std::is_unsigned_v<UnsignedType>, "Only works for unsigned types");
    v--;
    for (size_t i = 1; i < sizeof(v) * CHAR_BIT; i *= 2) // Prefer size_t "Warning comparison between signed and unsigned integer"
    {
        v |= v >> i;
    }
    return ++v;
}

static int PowerOf2Exponent(uint32_t value)
{
    if constexpr (sizeof(unsigned int) == sizeof value)
        return 31 - __builtin_clz(value);
    else if constexpr (sizeof(unsigned long) == sizeof value)
        return 31 - __builtin_clzl(value);
    else
        exit(127); /* Weird architecture! */
}

auto GetTextureClassType(const TexturePoolClass& a_TextureClass)
{
    switch (a_TextureClass) {
    case TexturePoolClass::Size128Bits:
        return GL_RGBA32F;
    case TexturePoolClass::Size96Bits:
        return GL_RGB32F;
    case TexturePoolClass::Size64Bits:
        return GL_RG32F;
    case TexturePoolClass::Size48Bits:
        return GL_RGB16;
    case TexturePoolClass::Size32Bits:
        return GL_RGBA8;
    case TexturePoolClass::Size24Bits:
        return GL_RGB8;
    case TexturePoolClass::Size16Bits:
        return GL_RG8;
    case TexturePoolClass::Size8Bits:
        return GL_R8;
    }
    return GL_NONE;
}

auto GetTexturePoolClass(const uint& a_InternalFormat)
{
    switch (a_InternalFormat) {
    case GL_RGBA32F:
    case GL_RGBA32UI:
    case GL_RGBA32I:
        return TexturePoolClass::Size128Bits;
    case GL_RGB32F:
    case GL_RGB32UI:
    case GL_RGB32I:
        return TexturePoolClass::Size96Bits;
    case GL_RGBA16F:
    case GL_RG32F:
    case GL_RGBA16UI:
    case GL_RG32UI:
    case GL_RGBA16I:
    case GL_RG32I:
    case GL_RGBA16:
    case GL_RGBA16_SNORM:
        return TexturePoolClass::Size64Bits;
    case GL_RG16F:
    case GL_R11F_G11F_B10F:
    case GL_R32F:
    case GL_RGB10_A2UI:
    case GL_RGBA8UI:
    case GL_RG16UI:
    case GL_R32UI:
    case GL_RGBA8I:
    case GL_RG16I:
    case GL_R32I:
    case GL_RGB10_A2:
    case GL_RGBA8:
    case GL_RG16:
    case GL_RGBA8_SNORM:
    case GL_RG16_SNORM:
    case GL_SRGB8_ALPHA8:
    case GL_RGB9_E5:
        return TexturePoolClass::Size32Bits;
    case GL_RGB8:
    case GL_RGB8_SNORM:
    case GL_SRGB8:
    case GL_RGB8UI:
    case GL_RGB8I:
        return TexturePoolClass::Size24Bits;
    case GL_R16F:
    case GL_RG8UI:
    case GL_R16UI:
    case GL_RG8I:
    case GL_R16I:
    case GL_RG8:
    case GL_R16:
    case GL_RG8_SNORM:
    case GL_R16_SNORM:
        return TexturePoolClass::Size16Bits;
    case GL_R8UI:
    case GL_R8I:
    case GL_R8:
    case GL_R8_SNORM:
        return TexturePoolClass::Size8Bits;
    }
    return TexturePoolClass::Unknown;
}

Texture2DPoolBlock::Texture2DPoolBlock(Context& a_context, const TexturePoolClass& a_TextureClass, const size_t& a_PowerOf2Exp)
    : textureClass(a_TextureClass)
    , powerOf2Exp(a_PowerOf2Exp)
    , _context(a_context)
{
    _context.PushImmediateCmd([this] {
        for (auto i = 0u; i < size; ++i) {
            auto placementPtr = (RAII::Texture2D*)_memory.at(i * sizeof(RAII::Texture2D));
            uint textureSize  = pow(2, powerOf2Exp);
            _textures.at(i)   = new (placementPtr) RAII::Texture2D(textureSize, textureSize, TexturePoolMaxLevels, GetTextureClassType(textureClass));
            _freeIndice.push(i);
        }
    });
}

Texture2DPoolBlock::~Texture2DPoolBlock()
{
    _context.PushImmediateCmd([this]() mutable {
        for (auto& texture : _textures)
            delete texture;
    });
}

uint8_t Texture2DPoolBlock::GetIndex()
{
    _UpdateLastUse();
    auto index = _freeIndice.back();
    _freeIndice.pop();
    return index;
}

void Texture2DPoolBlock::ReleaseIndex(const uint8_t& a_Index)
{
    _UpdateLastUse();
    _freeIndice.push(a_Index);
}

bool Texture2DPoolBlock::HasFreeIndex() const
{
    return !_freeIndice.empty();
}

bool Texture2DPoolBlock::NeedsFreeing() const
{
    return _freeIndice.size() == size
        && std::chrono::duration<double, std::milli>(std::chrono::system_clock::now() - _lastUse).count() > keepAliveDuration;
}

RAII::Texture2D& Texture2DPoolBlock::at(const size_t& a_Index)
{
    return *_textures.at(a_Index);
}

void TabGraph::Renderer::Texture2DPoolBlock::_UpdateLastUse()
{
    _lastUse = std::chrono::system_clock::now();
}

Texture2DPool::Texture2DPool(Context& a_Context)
    : _context(a_Context)
{
}

std::shared_ptr<RAII::Texture2D> Texture2DPool::Allocate(const uint& a_InternalFormat, const glm::uvec2& a_Size, const uint& a_Levels)
{
    const uint powerOf2Exp      = PowerOf2Exponent(NextPowerOf2(glm::max(a_Size.x, a_Size.y)));
    const auto texturePoolClass = GetTexturePoolClass(a_InternalFormat);
    auto& freeBlocksVector      = _freeBlocks[uint(texturePoolClass)][powerOf2Exp];
    std::shared_ptr<RAII::Texture2D> buffer;
    if (!freeBlocksVector.empty()) {
        // we have a pool !
        auto& block       = *freeBlocksVector.back();
        auto textureIndex = block.GetIndex();
        buffer            = std::shared_ptr<RAII::Texture2D>(
            &block.at(textureIndex),
            [this, block, textureIndex](RAII::Texture2D const*) mutable {
                _Free(&block, textureIndex);
            });
        if (!block.HasFreeIndex())
            freeBlocksVector.pop_back();
    } else {
        // no more free buffers for this size
        _AllocateBlock(texturePoolClass, powerOf2Exp);
        buffer = Allocate(a_InternalFormat, a_Size, a_Levels);
    }
    return buffer;
}

void Texture2DPool::Update()
{
    for (uint texturePool = 0; texturePool < uint(TexturePoolClass::MaxValue); ++texturePool) {
        for (uint powOf2 = 0; powOf2 < TexturePoolMaxPowerOf2Exp; ++powOf2) {
            auto& blocks  = _blocks[texturePool][powOf2];
            auto blockItr = blocks.begin();
            while (blockItr != blocks.end()) {
                if (blockItr->NeedsFreeing()) {
                    _freeBlocks[texturePool][powOf2].remove(&(*blockItr));
                    blockItr = blocks.erase(blockItr);
                } else
                    blockItr++;
            }
        }
    }
}

void Texture2DPool::_AllocateBlock(const TexturePoolClass& a_TexturePoolClass, const uint& a_TexturePowOf2Exp)
{
    _context.PushImmediateCmd([this, a_TexturePoolClass, a_TexturePowOf2Exp]() {
        auto& blocks = _blocks[uint(a_TexturePoolClass)][a_TexturePowOf2Exp];
        blocks.emplace_back(_context, a_TexturePoolClass, a_TexturePowOf2Exp);
        _freeBlocks[uint(a_TexturePoolClass)][a_TexturePowOf2Exp].emplace_back(&blocks.back());
    });
}

void Texture2DPool::_Free(Texture2DPoolBlock* a_Block, const uint& a_TextureIndex)
{
    a_Block->ReleaseIndex(a_TextureIndex);
    _freeBlocks[uint(a_Block->textureClass)][a_Block->powerOf2Exp].emplace_back(a_Block);
}
}