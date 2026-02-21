#pragma once

#include <cstdint>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include <glm/vec2.hpp>

namespace Msg {
class OGLContext;
class OGLTexture;
class OGLBindlessTextureSampler;
}

namespace Msg::Renderer {
class VirtualTexturePool;

struct VirtualTexturePage {
    VirtualTexturePage()                          = default;
    VirtualTexturePage(const VirtualTexturePage&) = default;
    VirtualTexturePage(VirtualTexturePool* a_Pool, const glm::uvec2& a_Address)
        : texturePool(a_Pool)
        , address(a_Address)
    {
    }
    VirtualTexturePool* texturePool = nullptr;
    glm::uvec2 address;
};

class VirtualTexturePool {
public:
    VirtualTexturePool(OGLContext& a_Ctx, const uint32_t& a_SizedFormat);
    uint64_t GetID();
    void UploadPage(const glm::uvec2& a_Page, const std::vector<std::byte>& a_Data);
    VirtualTexturePage RequestPage();
    void FreePage(const glm::uvec2& a_Page) { _freePages.push(a_Page); }
    bool Empty() const { return _freePages.empty(); }

private:
    std::shared_ptr<OGLTexture> _texture;
    std::shared_ptr<OGLBindlessTextureSampler> _textureSampler;
    std::queue<glm::uvec2> _freePages;
};

class VirtualTextureFormatAllocator {
public:
    VirtualTextureFormatAllocator(OGLContext& a_OGLContext, const uint32_t& a_Format);
    VirtualTexturePage RequestPage();
    void FreePage(const VirtualTexturePage& a_Page);
    void Shrink();
    bool Empty() const { return _pools.empty(); }

private:
    const uint32_t _format;
    OGLContext& _ctx;
    std::unordered_set<uint64_t> _freePools; // pools with free space left
    std::unordered_map<uint64_t, VirtualTexturePool> _pools;
};

class VirtualTextureAllocator {
public:
    VirtualTextureAllocator(OGLContext& a_OGLContext);
    VirtualTexturePage RequestPage(const uint32_t& a_SizedFormat);
    void FreePage(const uint32_t& a_SizedFormat, const VirtualTexturePage& a_Page);
    void Shrink();

private:
    OGLContext& _ctx;
    std::unordered_map<uint32_t, VirtualTextureFormatAllocator> _formatAllocators;
};
}