#pragma once

#include <chrono>
#include <list>
#include <unordered_map>
#include <vector>

#include <glm/vec4.hpp>

// Forward declarations
namespace Msg::Renderer {
class SparseTexture;
struct SparseTexturePageCacheKey;
}

// Hash function
namespace std {
template <typename T>
struct hash;
template <>
struct hash<Msg::Renderer::SparseTexturePageCacheKey> {
    size_t operator()(Msg::Renderer::SparseTexturePageCacheKey const& a_Value) const;
};
}

// Class declarations
namespace Msg::Renderer {
constexpr size_t SparseTexturePageCacheMaxSize        = 1073741824; // 1 Gb
constexpr std::chrono::minutes PageCacheLifeExpetency = std::chrono::minutes(1);
struct SparseTexturePageCacheData {
    std::vector<std::byte> rawData;
    std::chrono::system_clock::time_point lastAccess;
};
struct SparseTexturePageCacheKey {
    bool operator==(const SparseTexturePageCacheKey& a_Other) const
    {
        return a_Other.texture == texture
            && a_Other.pageIndex == pageIndex;
    }
    SparseTexture const* texture;
    uint32_t pageIndex;
};

class SparseTexturePageCache {
public:
    /** @brief removes unused pages */
    void Cleanup();
    void RemoveCache(const SparseTexture* a_Texture, const uint32_t& a_PageIndex);
    const std::vector<std::byte>* AddCache(const SparseTexture* a_Texture, const uint32_t& a_PageIndex, const std::vector<std::byte>& a_Data);
    const std::vector<std::byte>* GetCache(const SparseTexture* a_Texture, const uint32_t& a_PageIndex);

private:
    size_t _size = 0;
    std::list<SparseTexturePageCacheKey> _availableCache;
    std::unordered_map<SparseTexturePageCacheKey, SparseTexturePageCacheData> _cacheData;
};
}